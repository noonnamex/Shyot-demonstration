
/// Importy potrebných modulov pre NestJS a Mongoose
import { Module, forwardRef } from '@nestjs/common';
import { MongooseModule } from '@nestjs/mongoose';
import { DeviceService } from './device.service';
import { DeviceController } from './device.controller';
import { Device, DeviceSchema } from './device.model';  // Importovanie schémy zariadenia
import { DeviceData, DeviceDataSchema } from './device-data.model';
import { AuthModule } from '../auth/auth.module'; 
import { MqttModule } from '../mqtt/mqtt.module'; 

/// Definícia modulu pre zariadenia, ktorý obsahuje služby, kontroléry a schémy pre prácu so zariadeniami
@Module({
  imports: [
    /// Pripojenie schém pre zariadenia a údaje zariadení do Mongoose modulu
    MongooseModule.forFeature([
    { name: Device.name, schema: DeviceSchema },          // Pripojenie schémy Device do Mongoose
    { name: DeviceData.name, schema: DeviceDataSchema },  // Pripojenie schémy DeviceData do Mongoose
    ]),

    /// Importovanie iných modulov, ktoré sú závislé od tohto modulu (AuthModule, MqttModule)
    forwardRef(() => AuthModule), // Používa forwardRef na vytvorenie kruhových závislostí medzi AuthModule a DeviceModule
    forwardRef(() => MqttModule), // Podobne používa forwardRef pre MqttModule
  ],
  controllers: [DeviceController],            // Zoznam kontrolérov, ktoré obsluhujú HTTP požiadavky (v tomto prípade DeviceController)
  providers: [DeviceService],                 // Zoznam poskytovateľov služieb, ktoré budú k dispozícii v rámci modulu (v tomto prípade DeviceService)
  exports: [DeviceService, MongooseModule],   // Exportovanie služieb a Mongoose modulu pre použitie v iných moduloch
})
export class DeviceModule {}
