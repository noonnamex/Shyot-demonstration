#include <Arduino.h>
#include <Shyot.h>

/// WiFi údaje
char ssid[] = "HUAWEI-2bfe-2G";       
char password[] = "c6TaEjFU";

/// Shyot MQTT údaje
const char* user_token = "8727c96d5a3d4d6d754e8309733e627d72a0971ebc94060fe0cf4f459304740c";
const char* device_token = "l5j848ivpznpsil769cco";


/// pripojenie a dáta na STM32
uint8_t stmValues[7] = {1, 50, 100, 255, 128, 0, 0};  // init dáta
#define UART_TX_PIN 17                                // GPIO pin pre UART
HardwareSerial stmSerial(2);                          // Vytvorenie sériovej linky


// ==================== RECIEVE DATA CALLBACK FUNKCIA ====================

void pinChange(double value, const char* pin) {
    Serial.print("Zmena pinu: ");
    Serial.print(pin);
    Serial.print(" = ");
    Serial.println(value);
    
    int index = atoi(pin + 1);            // (D0) preskočí pin D a načíta číslo 0  
    if (index >= 0 && index < 7) {        // uloží hodnotu pinu na správne miesto
        stmValues[index] = (uint8_t)value;
        
        stmSerial.printf("SET,%d,%d,%d,%d,%d,%d,%d\n",
                        stmValues[0], stmValues[1], stmValues[2],
                        stmValues[3], stmValues[4], stmValues[5], stmValues[6]);
        
        Serial.println("Odoslané na STM32");
    }
}

// ==================== SETUP ====================

void setup() {
    Serial.begin(115200);                               // inicializovanie rýchlosti sériového portu
    delay(2000);
    
    // 1. UART
    stmSerial.begin(9600, SERIAL_8N1, -1, UART_TX_PIN); // inicializovanie UART
    delay(100);
    
    // 2. WiFi
    WiFiManager.begin(ssid, password);                  // inicializovanie wifi
    
    // 3. MQTT
    Shyot.begin(user_token, device_token);              // inicializovanie MQTT komunikácie
    
    // 4. Callbacky
    // registerCustom("pin", min, max, funkcia)
    Receiver.registerCustom("D0", 1, 3, pinChange);
    Receiver.registerCustom("D1", 0, 100, pinChange);
    Receiver.registerCustom("D2", 0, 100, pinChange);
    Receiver.registerCustom("D3", 0, 255, pinChange);
    Receiver.registerCustom("D4", 0, 255, pinChange);
    Receiver.registerCustom("D5", 0, 255, pinChange);
    Receiver.registerCustom("D6", 0, 255, pinChange);
    
    // 5. Počiatočné hodnoty
    stmSerial.printf("SET,%d,%d,%d,%d,%d,%d,%d\n",
                    stmValues[0], stmValues[1], stmValues[2],
                    stmValues[3], stmValues[4], stmValues[5], stmValues[6]);
    Serial.println("Systém pripravený.");
    Serial.println("Čakám 10s na WiFi...");
    delay(10000);
}

// ==================== LOOP ====================

void loop() {
    Shyot.loop();
}