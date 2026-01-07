Shyot: 
mobile app: flutter,riverpod,dart,mqtt,https,secure storage,MVVM,Android/iOS,TestFlight
backend: jwt authentification, brearer token, mongoDB, https, mqtt gateway, real-time
arduino ESP32/ESP8266: shyot library, non-blocking timers, automatic reconnect, heartbeat message, UART communication, Wifi komunikácia
STM32: výkonovo náročné operácie, pokročilé riadenie, DMA, PWM, Timers, ADC, low-level programming
Hardware: návrh vlastnej dosky pre ovládanie WS2815 adresovateľného 720 LED pásu

# Shyot - IoT

[![Flutter](https://img.shields.io/badge/Flutter-3.16-blue)](https://flutter.dev)
[![NestJS](https://img.shields.io/badge/Backend-NestJS-red)](https://nestjs.com)
[![ESP32](https://img.shields.io/badge/Hardware-ESP32/STM32-green)](https://www.espressif.com)
[![MongoDB](https://img.shields.io/badge/Database-MongoDB-brightgreen)](https://mongodb.com)
[![MQTT](https://img.shields.io/badge/Protocol-MQTT-orange)](https://mqtt.org)

**Kompletný IoT systém pre vzdialené ovládanie mikrokontrolérov pomocou mobilnej aplikácie a cloud backendu.**

---

## Mobilná Aplikácia (Flutter/Dart)

- Architektúra: MVVM s Riverpod pre stavový manažment
- Bezpečnosť: Secure Storage pre JWT tokeny, HTTPS komunikácia
- Realtime: MQTT klient pre okamžitú komunikáciu
- Cross-platform: Android/iOS s podporou TestFlight

---

## Backend Server (NestJS/TypeScript)

- Autentifikácia: JWT s bearer tokenmi
- Databáza: MongoDB pre ukladanie používateľských dát a nastavení
- Realtime Gateway: MQTT broker pre okamžitú komunikáciu s IoT zariadeniami
- API: RESTful API s HTTPS zabezpečením

---

## Embedded Systémy (C++)

- ESP32/ESP8266 (WiFi Connectivity)
- Vlastná shyot knižnica pre jednoduchú implementáciu
- Non-blocking timers pre plynulú prevádzku
- Automatické reconnection pri strate WiFi/MQTT
- Heartbeat správy pre monitoring online stavu
- UART komunikácia s STM32
- MQTT klient pre cloud komunikáciu

---

## STM32 (C)

- Pokročilé riadenie 720 WS2815 LED diód
- DMA (Direct Memory Access) pre plynulý prenos dát
- PWM generovanie pre presné ovládanie
- Low-level programovanie v C
- ADC (Analog-Digital Converter) pre snímanie prúdu
- Optimalizované časovače (Timers)
- UART komunikácia s nadradeným systémom

---

## Hardware & PCB Dizajn
- Návrh vlastnej dosky pre ovládanie WS2815 LED pásu
- Power management pre vysoký prúdový odber
- Signal integrity pre stabilný dátový prenos
- Over-current ochrana
- Optimalizované rozmiestnenie komponentov

---

## Hlavné funkcie
- Realtime ovládanie: Okamžitá reakcia na zmeny z mobilnej aplikácie
- Automatizácia: Časovače a scenáre pre inteligentné ovládanie
- Bezpečnosť: End-to-end zabezpečená komunikácia
- Scalability: Architektúra podporujúca viaceré zariadenia



















