
/// Importy potrebných modulov pre Mongoose a definície schémy
import { Prop, Schema, SchemaFactory } from '@nestjs/mongoose';
import { Document, Types } from 'mongoose';

/// Definícia typu DeviceDocument, ktorý rozširuje Device o funkcionalitu Mongoose Document
export type DeviceDocument = Device & Document;

/// Definícia schémy pre zariadenie v MongoDB
@Schema({ timestamps: true })  // Aktivuje automatické generovanie polí createdAt a updatedAt
export class Device {

  /// Automaticky generované ID zariadenia (Mongoose ObjectId)
  @Prop({ type: Types.ObjectId, default: () => new Types.ObjectId() })
  _id: Types.ObjectId;

  /// Vlastník zariadenia - email
  @Prop({ required: true })
  owner: string;

  /// Unikátny token zariadenia, ktorý je potrebný pre identifikáciu zariadenia
  @Prop({ required: true, unique: true })
  token: string; 

  /// Meno zariadenia (napr. "Kurník", "Termostat", atď.)
  @Prop()
  name: string; 
 
  /// Popis zariadenia, ktorý poskytuje ďalšie informácie
  @Prop()
  description: string; 

  /// Stav aktivity zariadenia (či je aktívne)
  @Prop({ default: true })
  isActive: boolean; 

  /// Stav online/offline zariadenia
  @Prop({ default: false })
  isOnline: boolean;

  /// Dátum, kedy zariadenie naposledy poslalo správu (ak je online)
  @Prop({ type: Date, default: null })
  lastSeen: Date;

  /// Uchovávanie aktuálnych stavov pinov zariadenia ako Map (kľúč: pin, hodnota: stav)
  @Prop({ type: Map, of: Number, default: {} })
  currentPins: Map<string, number>;
}

/// Vytvorenie schémy Mongoose pre triedu Device
export const DeviceSchema = SchemaFactory.createForClass(Device);