# Shyot - IoT

[![Flutter](https://img.shields.io/badge/Flutter-3.16-blue)](https://flutter.dev)
[![NestJS](https://img.shields.io/badge/Backend-NestJS-red)](https://nestjs.com)
[![ESP32](https://img.shields.io/badge/Hardware-ESP32/STM32-green)](https://www.espressif.com)
[![MongoDB](https://img.shields.io/badge/Database-MongoDB-brightgreen)](https://mongodb.com)
[![MQTT](https://img.shields.io/badge/Protocol-MQTT-orange)](https://mqtt.org)

**Kompletný IoT systém na vzdialené ovládanie mikrokontrolérov pomocou mobilnej aplikácie a cloud backendu.**

---

## Čo je Shyot?

Shyot je plnohodnotný IoT systém, ktorý umožňuje:

- **Ovládať ESP8266, ESP32 a STM32 zariadenia** z mobilu odkiaľkoľvek
- **Riadiť adresovateľné WS2815 LED pásky** s pokročilými efektmi pomocou PWM a DMA
- **Komunikovať bezpečne** cez MQTT a HTTPS
- **Ukladať históriu dát** v MongoDB cloudovej databáze
- **Spravovať viac zariadení** naraz v reálnom čase

---

## Architektúra Systému


## 🛠️ Technologický Stack

### **Mobilná Aplikácia (Flutter)**
- **Framework:** Flutter 3.16 + Dart 3.0
- **Architektúra:** MVVM s Riverpod state management
- **Funkcie:** Reálny čas, MQTT klient, HTTPS komunikácia
- **Platformy:** iOS a Android

### **Cloud Backend (NestJS)**
- **Framework:** NestJS 10.0 + TypeScript
- **Databáza:** MongoDB 7.0 s Mongoose ODM
- **Komunikácia:** MQTT broker, REST API, WebSocket
- **Infraštruktúra:** Linode VPS, Docker, Nginx + SSL

### **Embedded Systémy**
- **Mikrokontroléry:** ESP8266, ESP32, STM32
- **Jazyk:** C++ (Arduino framework)
- **Periférie:** WS2815 LED, PWM, DMA, GPIO
- **Nástroje:** PlatformIO, VS Code

### **Bezpečnosť a Protokoly**
- **Šifrovanie:** HTTPS/TLS pre všetku komunikáciu
- **Autentifikácia:** JWT tokeny, certifikáty zariadení
- **Protokoly:** MQTT 3.1.1, HTTP/2, WebSocket

---

## 🚀 Kľúčové Funkcie

### **Mobilná Aplikácia**
- Dashboard v reálnom čase s MVVM architektúrou
- Pokročilé ovládanie LED pások (WS2815) s efektami
- Konfigurácia PWM a DMA parametrov
- Automatizácia pomocou časovačov
- Zabezpečená MQTT komunikácia s QoS podporou
- Správa viacerých zariadení (ESP8266/32, STM32)

### **Backend Server**
- Škálovateľný MQTT broker pre 1000+ zariadení
- Ukladanie časových radov do MongoDB
- Device provisioning a správa certifikátov
- OTA (Over-The-Air) aktualizácie firmware
- Real-time monitoring a analýzy

### **Hardvérová Integrácia**
- Vlastná Shyot knižnica pre ESP platformy
- Ovládanie WS2815 LED pások s gamma korekciou
- Pokročilé PWM s DMA pre plynulé animácie
- Komunikácia so STM32 cez sériový protokol
- Nízko-energetické režimy a wake-up mechanizmy




