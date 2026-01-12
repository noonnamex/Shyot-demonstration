# Shyot - IoT

[![Flutter](https://img.shields.io/badge/Flutter-Dart-blue)](https://flutter.dev)
[![NestJS](https://img.shields.io/badge/Backend-NestJS-red)](https://nestjs.com)
[![ESP32](https://img.shields.io/badge/Hardware-ESP32/STM32-green)](https://www.espressif.com)
[![MongoDB](https://img.shields.io/badge/Database-MongoDB-brightgreen)](https://mongodb.com)
[![MQTT](https://img.shields.io/badge/Protocol-MQTT-orange)](https://mqtt.org)


**Kompletný IoT systém pre vzdialené ovládanie mikrokontrolérov pomocou mobilnej aplikácie a cloud backendu.**



## 🎥 Video Demo
[![Shyot Demonstration](https://img.shields.io/badge/🎬_Watch_Video-Shyot_Demonstration-red)](https://www.youtube.com/watch?v=pQ44LPSa9n4)


Video ukazuje kompletné fungovanie systému od mobilnej aplikácie po fyzické projekty. K lepšej predstave pomôže (![Flowchart systému](Shyot%20flowchart.png))



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
- Vlastná shyot knižnica pre jednoduchú integráciu
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
- UART komunikácia s nadradeným systémom Simplex

---

## Hardware & PCB Dizajn
- Návrh vlastnej dosky pre ovládanie WS2815 LED pásu
- Power management pre vysoký prúdový odber
- Signal integrity pre stabilný dátový prenos
- Over-current ochrana
- Optimalizované rozmiestnenie komponentov

---

## Hlavné funkcie
- Realtime ovládanie: Okamžitá obojsmerná komunikácia medzi procesorom a aplikáciou
- Automatizácia: Časovače a scenáre pre inteligentné ovládanie
- Bezpečnosť: End-to-end zabezpečená komunikácia
- Scalability: Architektúra podporujúca viaceré zariadenia

---

## Kontakt

Otvorím sa pre príležitosti v oblasti:
- **Full-Stack vývoj** (Flutter + Backend)
- **Embedded systémy & IoT**
- **Cross-platform mobilných aplikácií**
- **Hardware/Software integrácie**

📧 **Email**: samuelmatus@centrum.sk 
---
















