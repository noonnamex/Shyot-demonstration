
/// Importy potrebných knižníc a komponentov
import { Injectable, NotFoundException, Logger } from '@nestjs/common';
import { InjectModel } from '@nestjs/mongoose';
import { Model, Types } from 'mongoose';
import { Device, DeviceDocument } from './device.model';
import { DeviceData, DeviceDataDocument } from './device-data.model';
import { User, UserDocument } from '../auth/auth.model';  
import { InjectRedis } from '@nestjs-modules/ioredis';
import Redis from 'ioredis';
import { Cron } from '@nestjs/schedule';

/// Hlavná služba pre správu zariadení
@Injectable()
export class DeviceService {
  private readonly logger = new Logger(DeviceService.name);
  private readonly OFFLINE_THRESHOLD = 600;                   // Prah pre offline zariadenia v sekundách (10 minút)

  constructor(
    @InjectModel(Device.name) private deviceModel: Model<DeviceDocument>,                 // Model pre zariadenia
    @InjectModel(DeviceData.name) private deviceDataModel: Model<DeviceDataDocument>,     // Model pre údaje zariadení
    @InjectModel(User.name) private userModel: Model<UserDocument>,                       // Model pre používateľov
    @InjectRedis() private readonly redis: Redis                                          // Redis pre rýchle operácie
  ) {}

  /// Získa zariadenia používateľa podľa jeho emailu
  async getUserDevices(email: string): Promise<Device[]> {
    this.logger.log(`Fetching devices for user: ${email}`);
    
    try {
      const devices = await this.deviceModel
        .find({ owner: email })                                                             // Získaj zariadenia pre daný email
        .select('name description token isActive isOnline lastSeen deviceType location')    // Vyber konkrétne polia
        .sort({ isOnline: -1, name: 1 })                                                    // Triedenie zariadení podľa stavu a názvu
        .exec();
      
      this.logger.log(`Found ${devices.length} devices for user: ${email}`);
      return devices;
    } catch (error) {
      this.logger.error(`Error fetching devices for user ${email}:`, error.stack);
      throw error;                                                                          // Vyvolá chybu, ak zlyhá načítanie zariadení
    }
  }




  /// Získa zariadenia používateľa so všetkými dátami (vrátane stavu pinov)
  async getUserDevicesWithData(userId: string): Promise<any> {
    this.logger.log(`Fetching user devices with data for user ID: ${userId}`);
    
    try {
      // 1. Získa používateľa podľa ID a jeho email
      const user = await this.userModel
        .findById(userId)
        .select('email')
        .lean()
        .exec();

      if (!user) {
        throw new NotFoundException('User not found');  // Ak používateľ neexistuje, vyvolaj výnimku
      }

      // 2. zariadenia podľa emailu (majiteľa)
      const devices = await this.deviceModel
        .find({ owner: user.email })  // hľadá podľa owner (emailu)
        .select('token name description isOnline lastSeen currentPins isActive color')
        .lean()
        .exec();

      // 3. Vráti štruktúrovanú odpoveď s údajmi o zariadeniach
      return {
        userId: user._id,
        email: user.email,
        devices: devices.map(device => ({
          id: device._id,
          token: device.token,
          name: device.name,
          description: device.description,
          isActive: device.isActive,
          isOnline: device.isOnline,
          lastSeen: device.lastSeen,
          currentPins: device.currentPins || {},
        }))
      };
    } catch (error) {
      this.logger.error(`Error fetching user devices with data for user ${userId}:`, error.stack);
      throw error;    // Vyvolá chybu v prípade zlyhania
    }
  }


  /// Spracovanie heartbeat správ od zariadení (informácie o poslednej aktivite)
  async handleHeartbeatFromDevice(token: string): Promise<void> {
    try {
      // Okamžitá aktualizácia v Redis
      await this.redis.set(`device:${token}:lastSeen`, Date.now());
      await this.redis.sadd('online:devices', token);
      
      this.logger.debug(`💓 Heartbeat from ${token} - device is alive`);

      // Asynchrónna aktualizácia v MongoDB
      this.updateMongoDBLastSeen(token).catch(error => {
        this.logger.error(`MongoDB update failed for ${token}: ${error.message}`);
      });

    } catch (error) {
      this.logger.error(`Heartbeat processing failed for ${token}: ${error.message}`);
    }
  }


  /// Aktualizuje poslednú aktivitu zariadenia v Redis a MongoDB
  async updateDeviceLastSeen(token: string): Promise<void> {
    try {
      // Okamžitá aktualizácia v Redis
      const redisKey = `device:${token}:lastSeen`;
      await this.redis.set(redisKey, Date.now());
      await this.redis.sadd('online:devices', token); // Pridá do online setu
      
      this.logger.debug(`🔄 Redis updated for device ${token}`);

      // Asynchrónna aktualizácia v MongoDB
      this.updateMongoDBLastSeen(token).catch(error => {
        this.logger.error(`MongoDB update failed for ${token}: ${error.message}`);
      });

    } catch (error) {
      this.logger.error(`Redis update failed for ${token}: ${error.message}`);
      // Fallback: Ak Redis zlyhá, použijeme len MongoDB
      await this.updateMongoDBLastSeen(token);
    }
  }

  /// Pomocná funkcia na aktualizáciu MongoDB (pomalšie, ale trvalé)
  private async updateMongoDBLastSeen(token: string): Promise<void> {
    await this.deviceModel.updateOne(
      { token },
      { 
        $set: { 
          lastSeen: new Date(),
          isOnline: true 
        } 
      }
    ).exec();
    this.logger.debug(`📀 MongoDB updated for device ${token}`);
  }


  /// Kontrola online stavu zariadenia cez Redis (rýchla odpoveď)
  async isDeviceOnline(token: string): Promise<boolean> {
    try {
      const lastSeen = await this.redis.get(`device:${token}:lastSeen`);
      if (!lastSeen) {
        // Ak nie je v Redis, skontrolujeme MongoDB
        return await this.checkMongoDBOnlineStatus(token);  // Ak nie je v Redis, skontroluj MongoDB
      }

      const lastSeenTime = parseInt(lastSeen);
      const now = Date.now();
      const isOnline = (now - lastSeenTime) < (this.OFFLINE_THRESHOLD * 1000);
      
      return isOnline;
    } catch (error) {
      this.logger.error(`Redis check failed for ${token}: ${error.message}`);
      // Fallback na MongoDB
      return await this.checkMongoDBOnlineStatus(token);
    }
  }

  /// Pomocná funkcia na kontrolu online stavu zariadenia cez MongoDB
  private async checkMongoDBOnlineStatus(token: string): Promise<boolean> {
    const device = await this.deviceModel.findOne({ token }).exec();
    if (!device || !device.lastSeen) return false;

    const now = Date.now();
    const lastSeenTime = device.lastSeen.getTime();
    return (now - lastSeenTime) < (this.OFFLINE_THRESHOLD * 1000);
  }


  /// Kontrola online stavu všetkých zariadení (každých 5 minút)
  @Cron('*/5 * * * *') // Spustí sa každých 5 minút
  async checkDevicesOnlineStatus(): Promise<void> {
    try {
      // Rýchla kontrola online zariadení v Redis
      const deviceTokens = await this.redis.smembers('online:devices');
      const now = Date.now();
      let offlineCount = 0;

      const pipeline = this.redis.pipeline();
      const offlineTokens: string[] = [];
      
      for (const token of deviceTokens) {
        const lastSeen = await this.redis.get(`device:${token}:lastSeen`);
        if (lastSeen) {
          const lastSeenTime = parseInt(lastSeen);
          if ((now - lastSeenTime) > (this.OFFLINE_THRESHOLD * 1000)) {
            // Zariadenie je offline
            pipeline.srem('online:devices', token);
            offlineTokens.push(token);
            offlineCount++;
          }
        }
      }

      // Spustíme všetky Redis operácie naraz
      if (offlineTokens.length > 0) {
        await pipeline.exec();
        this.logger.log(`🔍 Redis marked ${offlineCount} devices as offline`);
      }

      // Synchronizácia offline zariadení do MongoDB (asynchrónne)      
      if (offlineTokens.length > 0) {
        this.syncOfflineDevicesToMongoDB(offlineTokens).catch(error => {
          this.logger.error(`MongoDB sync failed: ${error.message}`);
        });
      }

    } catch (error) {
      this.logger.error(`Redis check failed: ${error.message}`);
      // Fallback: Ak Redis zlyhá, použijeme len MongoDB
      await this.checkDevicesOnlineStatusMongoDBFallback();
    }
  }

  /// Pomocná funkcia na synchronizáciu offline zariadení do MongoDB
  private async syncOfflineDevicesToMongoDB(offlineTokens: string[]): Promise<void> {
    await this.deviceModel.updateMany(
      { 
        token: { $in: offlineTokens },
        isOnline: true 
      },
      { $set: { isOnline: false } }
    ).exec();
    
    this.logger.debug(`📀 MongoDB synced - marked ${offlineTokens.length} devices as offline`);
  }

  /// Fallback kontrola stavu zariadení v MongoDB
  private async checkDevicesOnlineStatusMongoDBFallback(): Promise<void> {
    const twoMinutesAgo = new Date(Date.now() - 2 * 60 * 1000);
    
    const result = await this.deviceModel.updateMany(
      { 
        lastSeen: { $lt: twoMinutesAgo }, 
        isOnline: true 
      },
      { $set: { isOnline: false } }
    ).exec();
    
    if (result.modifiedCount > 0) {
      this.logger.log(`🔍 MongoDB fallback marked ${result.modifiedCount} devices as offline`);
    }
  }

  /// Získanie online stavu zariadenia s detailmi
  async getDeviceOnlineStatus(token: string): Promise<{ 
    isOnline: boolean; 
    lastSeen: Date | null;
    source: 'redis' | 'mongodb';
  }> {
    try {
      // Skúsime najprv Redis
      const lastSeenRedis = await this.redis.get(`device:${token}:lastSeen`);
      if (lastSeenRedis) {
        const lastSeenTime = parseInt(lastSeenRedis);
        const isOnline = (Date.now() - lastSeenTime) < (this.OFFLINE_THRESHOLD * 1000);
        
        return {
          isOnline,
          lastSeen: new Date(lastSeenTime),
          source: 'redis'
        };
      }
    } catch (error) {
      this.logger.debug(`Redis check failed for ${token}, falling back to MongoDB`);
    }

    // Fallback na MongoDB
    const device = await this.deviceModel.findOne({ token }).exec();
    return {
      isOnline: device ? this.isDeviceOnlineInMongoDB(device) : false,
      lastSeen: device?.lastSeen || null,
      source: 'mongodb'
    };
  }

  /// Pomocná funkcia na kontrolu online stavu z MongoDB dokumentu
  private isDeviceOnlineInMongoDB(device: DeviceDocument): boolean {
    if (!device.lastSeen) return false;
    return (Date.now() - device.lastSeen.getTime()) < (this.OFFLINE_THRESHOLD * 1000);
  }

  /// Inicializácia Redis pri spustení aplikácie
  async initializeRedisFromMongoDB(): Promise<void> {
    try {
      const onlineDevices = await this.deviceModel.find({ 
        isOnline: true,
        lastSeen: { $gte: new Date(Date.now() - this.OFFLINE_THRESHOLD * 1000) }
      }).exec();

      const pipeline = this.redis.pipeline();
      
      for (const device of onlineDevices) {
        pipeline.set(`device:${device.token}:lastSeen`, device.lastSeen.getTime());
        pipeline.sadd('online:devices', device.token);
      }

      await pipeline.exec();
      this.logger.log(`🎯 Redis initialized with ${onlineDevices.length} online devices from MongoDB`);
    } catch (error) {
      this.logger.error(`Redis initialization failed: ${error.message}`);
    }
  }


  /// Vytvorí nové zariadenie pre používateľa (podľa emailu)
  async createDevice(
    email: string,     
    name: string, 
    description: string = ''
  ): Promise<Device> {
    const token = this.generateDeviceToken();
    
    const device = new this.deviceModel({
      owner: email, // Použijeme email ako vlastníka zariadenia
      name,
      description,
      token,
      isActive: true,
      isOnline: false,
    });

    const savedDevice = await device.save();
    this.logger.log(`Device "${name}" created for user ${email} with token: ${token}`);
    
    return savedDevice;
  }

  /// Funkcia na generovanie tokenu pre zariadenie
  private generateDeviceToken(): string {
    return Math.random().toString(36).substring(2, 15) + Math.random().toString(36).substring(2, 15);
  }

  /// Získanie všetkých zariadení pre konkrétneho používateľa
  async getDevicesByUser(userId: string) {
    return this.deviceModel.find({ userId: new Types.ObjectId(userId) }).exec();
  }

  /// Nájde zariadenie podľa jeho tokenu
  async getDeviceByToken(token: string): Promise<Device | null> {
    return await this.deviceModel.findOne({ token })
      .populate('owner')   // Načítanie informácií o vlastníkovi zariadenia
      .exec();
  }

  /// Aktualizovanie zariadenia
  async updateDevice(token: string, updates: Partial<Device>): Promise<Device | null> {
    return await this.deviceModel.findOneAndUpdate(
      { token },
      { ...updates },
      { new: true }
    ).exec();
  }

  /// Aktualizácia stavu pinov zariadenia podľa tokenu
  async updateCurrentPinByToken(token: string, pin: string, value: number) {
    const device = await this.deviceModel.findOneAndUpdate(
      { token },
      { $set: { [`currentPins.${pin}`]: value } },
      { new: true }
    ).exec();

    if (!device) throw new NotFoundException('Device not found');

    return device;
  }

  /// Uloženie historických údajov o zariadení
  async saveDeviceData(deviceId: Types.ObjectId | string, pin: string, value: number) {
    const deviceObjectId = typeof deviceId === 'string' ? new Types.ObjectId(deviceId) : deviceId;
    const entry = new this.deviceDataModel({
      deviceId: deviceObjectId,
      pin,
      value,
      timestamp: new Date(),
    });
    return entry.save();
  }

  /// Aktualizácia stavu zariadenia (online/offline)
  async updateDeviceStatus(token: string, isOnline: boolean): Promise<void> {
  try {
    await this.deviceModel.findOneAndUpdate(
      { token },
      { 
        isOnline,
        lastSeen: isOnline ? new Date() : undefined 
      }
    );
    this.logger.log(`Device ${token} status updated to: ${isOnline ? 'online' : 'offline'}`);
  } catch (error) {
    this.logger.error(`Error updating device status for token ${token}:`, error.stack);
  }
}


/// Odstránenie zariadenia podľa tokenu
async deleteDevice(token: string, userEmail: string): Promise<any> {
  this.logger.log(`Deleting device ${token} for user ${userEmail}`);
  
  // 1. Nájdi zariadenie a skontroluj vlastníctvo
  const device = await this.deviceModel.findOne({ 
    token: token,
    owner: userEmail 
  }).exec();

  if (!device) {
    throw new NotFoundException('Device not found or you do not have permission to delete it');
  }

  const deviceId = device._id;

  // 2. Vymaž zariadenie z Redis
  try {
    await this.redis.del(`device:${token}:lastSeen`);
    await this.redis.srem('online:devices', token);
    this.logger.debug(`🗑️ Redis cleaned for device ${token}`);
  } catch (error) {
    this.logger.error(`Redis cleanup failed for ${token}: ${error.message}`);
  }

  // 3. Vymaž zariadenie z MongoDB
  await this.deviceModel.deleteOne({ _id: deviceId }).exec();
  
  // 4. Vymaž historické dáta zariadenia
  await this.deviceDataModel.deleteMany({ deviceId: deviceId }).exec();
  
  // 5. Odstráň zariadenie z user.devices poľa
  await this.userModel.updateOne(
    { email: userEmail },
    { $pull: { devices: deviceId } }
  ).exec();

  this.logger.log(`✅ Device ${token} deleted successfully`);

  return {
    deletedDeviceId: deviceId,
    token: token,
    name: device.name
  };
}


}