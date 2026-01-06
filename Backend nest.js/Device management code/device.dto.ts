
/// Importy potrebných dekorátorov z knižnice class-validator pre validáciu dát
import { IsString, IsOptional, IsNotEmpty } from 'class-validator';

/// Definícia DTO (Data Transfer Object) pre vytvorenie zariadenia
export class CreateDeviceDto {

  /// Názov zariadenia, povinný reťazec
  @IsString()     // Overenie, že hodnota je reťazec
  @IsNotEmpty()   // Overenie, že reťazec nie je prázdny
  name: string;   // Meno zariadenia

  @IsOptional()           // Označuje, že tento atribút je voliteľný
  @IsString()             // Overenie, že hodnota je reťazec
  description?: string;   // Popis zariadenia (nepovinné)
  
}
