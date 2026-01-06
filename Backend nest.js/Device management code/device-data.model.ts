
/// Importy potrebných modulov z knižníc
import { Prop, Schema, SchemaFactory } from '@nestjs/mongoose';
import { Document, Types } from 'mongoose';

/// Definícia typu pre dokument DeviceData, ktorý bude rozšírený o funkcionalitu Mongoose Document
export type DeviceDataDocument = DeviceData & Document;

/// Definícia schémy pre kolekciu DeviceData v MongoDB
@Schema({ timestamps: true })
export class DeviceData {

  /// Referencia na dokument v kolekcii "Device", id zariadenia, ktoré tento záznam generuje
  @Prop({ type: Types.ObjectId, required: true, index: true, ref: 'Device' })
  deviceId: Types.ObjectId;        // referencuje dokument zariadenia

  /// Pin, ktorý reprezentuje fyzický pin na zariadení (napr. "D5" znamená digitálny pin 5)
  @Prop({ required: true, index: true })
  pin: string;                     // e.g. "D5"

  /// Hodnota zobrazená zo zariadenia (číslo, ktoré predstavuje stav)
  @Prop({ required: true })
  value: number;                   // hodnota pinu

  /// Timestamp, kedy bol záznam vytvorený alebo upravený (predvolený čas je nastavený na aktuálny čas)
  @Prop({ type: Date, default: () => new Date(), index: true })
  timestamp: Date;
}

/// Vytvorenie schémy pre Mongoose z triedy DeviceData
export const DeviceDataSchema = SchemaFactory.createForClass(DeviceData);
