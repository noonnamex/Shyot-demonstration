/// Importy potrebných modulov z knižníc
import { Controller, Post, Get, Delete, Body, Param, Request, UseGuards, Patch, Req, HttpStatus, Logger, NotFoundException } from '@nestjs/common';
import { DeviceService } from './device.service';
import { JwtAuthGuard } from '../auth/jwt-auth.guard';
import { CreateDeviceDto } from './device.dto';
import { Device } from './device.model';
import { MqttGateway } from '../mqtt/mqtt.gateway'; 


/// Definícia DeviceController triedy, ktorá obsahuje rôzne endpointy pre manipuláciu so zariadeniami
@Controller('devices')
export class DeviceController {
  private readonly logger = new Logger(DeviceController.name);  // Vytvorenie logovacieho objektu pre túto triedu

  constructor(
    private readonly deviceService: DeviceService,  // Služba na manipuláciu s dátami o zariadeniach
    private readonly mqttGateway: MqttGateway, // MQTT gateway pre komunikáciu s pripojenými zariadeniami
  ) {}


  /**
   * Endpoint na získanie všetkých zariadení používateľa
   * Chránený JWT guardom - vyžaduje platný token
   * GET https://shyot.com/devices
   */
  @UseGuards(JwtAuthGuard)
  @Get()
  async getUserDevices(@Req() req) {
    const email = req.user.email;     // Získanie emailu používateľa z JWT tokenu
    this.logger.log(`GET /devices - Fetching devices for user: ${email}`);
    
    try {
      const devices = await this.deviceService.getUserDevices(email);   // Zavolanie služby na získanie zariadení
      
      return {
        statusCode: HttpStatus.OK,
        message: 'Devices retrieved successfully',
        data: devices,   // Vrátenie zoznamu zariadení
      };
    } catch (error) {
      this.logger.error(`Error in GET /devices for user ${email}:`, error.stack);
      
      return {
        statusCode: HttpStatus.INTERNAL_SERVER_ERROR,
        message: 'Failed to retrieve devices',
        error: error.message,     // Vrátenie chyby, ak sa zlyhá získanie zariadení
      };
    }
  }


  /// Endpoint na získanie zariadení používateľa s údajmi (chránený JWT auth)
  @UseGuards(JwtAuthGuard)
  @Get('my-devices')
  async getMyDevices(@Req() req) {
    const userId = req.user.userId;   // Získanie ID používateľa z JWT
    const email = req.user.email;     // Získanie emailu používateľa z JWT
    
    this.logger.log(`GET /devices/my-devices - Fetching devices for user: ${email} (ID: ${userId})`);
    
    try {
      const userData = await this.deviceService.getUserDevicesWithData(userId);   // Zavolanie služby na získanie zariadení s údajmi
      
      return {
        statusCode: HttpStatus.OK,
        message: 'Devices retrieved successfully',
        data: userData,    // Vrátenie údajov o zariadeniach používateľa
      };
    } catch (error) {
      this.logger.error(`Error in GET /devices/my-devices for user ${email}:`, error.stack);
      
      return {
        statusCode: HttpStatus.INTERNAL_SERVER_ERROR,
        message: 'Failed to retrieve devices',
        error: error.message,   // Vrátenie chyby, ak sa zlyhá získanie zariadení
      };
    }
  }


  /// Endpoint na vytvorenie nového zariadenia (chránený JWT auth)  @UseGuards(JwtAuthGuard)
  @Post()
  async createDevice(@Req() req, @Body() createDeviceDto: CreateDeviceDto) {
    const email = req.user.email;                     // Získanie emailu používateľa z JWT
    const { name, description } = createDeviceDto;    // Extrakcia dát zo vstupného DTO
    
    this.logger.log(`POST /devices - Creating device "${name}" for user: ${email}`);
    
    try {
      const device = await this.deviceService.createDevice(email, name, description);   // Zavolanie služby na vytvorenie zariadenia
      
      return {
        statusCode: HttpStatus.CREATED,
        message: 'Device created successfully',
        data: {        // Vrátenie údajov o vytvorenom zariadení
          id: device._id,
          name: device.name,
          token: device.token,
          description: device.description,
        },
      };
    } catch (error) {
      this.logger.error(`Error in POST /devices for user ${email}:`, error.stack);
      
      return {
        statusCode: HttpStatus.INTERNAL_SERVER_ERROR,
        message: 'Failed to create device',
        error: error.message,   // Vrátenie chyby, ak sa zlyhá vytvorenie zariadenia
      };
    }
  }


  /// Endpoint na získanie zariadenia podľa tokenu (nie je chránený JWT auth)
  @Get(':token')
  async getDeviceByToken(@Param('token') token: string) {
    this.logger.log(`GET /devices/${token} - Fetching device by token`);
    
    const device = await this.deviceService.getDeviceByToken(token);          // Zavolanie služby na získanie zariadenia podľa tokenu
    if (!device) {
      throw new NotFoundException(`Device with token '${token}' not found`);  // Hodí výnimku, ak zariadenie neexistuje
    }
    
    return {
      statusCode: HttpStatus.OK,
      message: 'Device retrieved successfully',
      data: device,   // Vrátenie údajov o zariadení
    };
  }


  /// Endpoint na aktualizáciu zariadenia podľa tokenu (chránený JWT auth)
  @UseGuards(JwtAuthGuard)
  @Patch(':token')
  async updateDevice(
    @Param('token') token: string,    // Získanie tokenu zariadenia z parametra URL
    @Body() updates: Partial<Device>  // Očakáva sa objekt s aktualizovanými hodnotami
  ) {
    return this.deviceService.updateDevice(token, updates);   // Zavolanie služby na aktualizáciu zariadenia
  }

  
  /// Endpoint na aktualizáciu pinov zariadenia (chránený JWT auth)
  @UseGuards(JwtAuthGuard)
  @Patch(':token/pins')
  async updatePins(
    @Param('token') token: string,              // Získanie tokenu zariadenia z parametra URL
    @Body() pinValues: Record<string, number>,  // Očakáva objekt s pinmi a hodnotami
  ) {
    // Pre každý pin a jeho hodnotu sa pošle príkaz cez MqttGateway
    for (const pin in pinValues) {
      const value = pinValues[pin];
      await this.mqttGateway.updatePinValue(token, pin, value);   // Posielanie príkazu na aktualizáciu pinov cez MQTT
    }

    return { message: 'Pins updated successfully' };              // Vrátenie správy o úspešnej aktualizácii pinov
  }


  /// Endpoint na vymazanie zariadenia podľa tokenu (chránený JWT auth)
  @UseGuards(JwtAuthGuard)
  @Delete(':token')
  async deleteDevice(
    @Param('token') token: string,  // Získanie tokenu zariadenia z parametra URL
    @Req() req                      // Získanie informácií o používateľovi z požiadavky
  ) {
    const email = req.user.email;   // Získanie emailu používateľa z JWT
    
    this.logger.log(`DELETE /devices/${token} - Deleting device for user: ${email}`);  // Zavolanie služby na vymazanie zariadenia
    
    try {
      const result = await this.deviceService.deleteDevice(token, email);
      
      return {
        statusCode: HttpStatus.OK,
        message: 'Device deleted successfully',
        data: result,           // Vrátenie výsledku vymazania zariadenia
      };
    } catch (error) {
      this.logger.error(`Error in DELETE /devices/${token} for user ${email}:`, error.stack);
      
      return {
        statusCode: HttpStatus.INTERNAL_SERVER_ERROR,
        message: 'Failed to delete device',
        error: error.message,   // Vrátenie chyby, ak sa zlyhá vymazanie zariadenia
      };
    }
  }



}
