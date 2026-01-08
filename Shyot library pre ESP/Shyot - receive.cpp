// **************************************************************
//                            RECEIVE DATA
// **************************************************************

// ====== USECASE ======
/*
#include <Arduino.h>
#include <Shyot.h>

// 1. Definuj si LEDToggle funkciu
void ledToggle(double value, const char* pin) {
    // value = 0 alebo 1
    // pin = "D0"
    
    if (value == 1) {                   // Ak je tlačidlo stlačené
        static bool ledState = false;   // Stav LED
        ledState = !ledState;           // Prepni
        
        digitalWrite(LED_BUILTIN, ledState ? HIGH : LOW);
        Serial.print("LED: ");
        Serial.println(ledState ? "🔆 ZAP" : "🌙 VYP");
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    
    WiFiManager.begin("WiFi", "Heslo");
    Shyot.begin("user", "device");
    
    // 2. POUŽIJ LEDToggle funkciu
    // registerCustom("pin", min, max, funkcia);
    Receiver.registerCustom("D0", 0, 1, ledToggle);
    
    Serial.println("Stlač D0 v appke na toggle LED!");
}

void loop() {
    Shyot.loop();
    delay(50);
}
*/
// ====== USECASE ======


#if defined(ESP8266) || defined(ESP32)
extern PubSubClient mqttClient;     // Deklarácia externého MQTT klienta
#endif

ReceiveData Receiver;               // vytvorenie objektu Reciever


// **************************************************************
//                 REGISTRÁCIE KOMPONENTOV
// **************************************************************

/// Funkcia na registráciu callbacku
/// Volá sa v setup() aby si povedal: "Keď príde správa pre tento pin, zavolaj príslušnú moju funkciu"
void ReceiveData::registerCustom(const char* pin, double min, double max, std::function<void(double,const char*)> callback) {
    
    // Vytvorenie nového objektu
    ControlCallback cb;

    // Rozdelenie a uloženie zadaných hodnôt funkcia, max, min
    cb.function = callback;
    cb.minValue = min;
    cb.maxValue = max;

    // Pridá do zoznamu: {"D0", {funkcia, 1, 3}}
    _callbacks.push_back({String(pin), cb});

    // výpis
    Serial.printf("Registered custom: %s (%.2f–%.2f)\n", pin, min, max);
}



// **************************************************************
//                     HANDLE MQTT MESSAGE
// **************************************************************

/// Táto funkcia sa automaticky volá keď príde MQTT správa
void ReceiveData::handleMessage(char* topic, byte* payload, unsigned int length) {
    
    // Vytvorí sa String z prijatých dát
    String msg;
    for (unsigned int i = 0; i < length; i++) {
        msg += (char)payload[i];    // Bajty sa premenia na znaky
    }

    // Výpis
    Serial.printf("Received on topic %s: %s\n", topic, msg.c_str());

    // Konverzia topic na String
    String topicStr = String(topic);

    // Skontroluj či je to správny formát topicu od Flutter appky
    // Formát: "frontend/USER_TOKEN/DEVICE_TOKEN/command"
    if (topicStr.startsWith("frontend/") && topicStr.endsWith("/command")) {
        Serial.println("✅ Detected Flutter command format");
        
        // Extrahuj device token z topicu: frontend/MQTT_TOKEN/DEVICE_TOKEN/command
        String topicParts[4];  // Pole pre 4 časti
        int partIndex = 0;     // Index aktuálnej časti
        int lastIndex = 0;     // Pozícia poslednej lomky
        
        for (int i = 0; i < topicStr.length() && partIndex < 4; i++) {
            if (topicStr[i] == '/') {
                
                // Vezmi časť medzi poslednou a aktuálnou lomkou
                topicParts[partIndex] = topicStr.substring(lastIndex, i);
                lastIndex = i + 1;  // Posuň sa za lomku
                partIndex++;        // Ďalšia časť
            }
        }

        // Zober poslednú časť (za poslednou lomkou)
        if (partIndex < 4) {
            topicParts[partIndex] = topicStr.substring(lastIndex);
        }
        
        // Tretia časť je device token (frontend/USER_TOKEN/DEVICE_TOKEN/command)
        String userTokenFromTopic = topicParts[1];      // USER_TOKEN z topicu
        String deviceTokenFromTopic = topicParts[2];    // DEVICE_TOKEN
        
        Serial.print("Extracted device token: ");
        Serial.println(deviceTokenFromTopic);
        Serial.print("My device token: ");
        Serial.println(_device_token);
        
        // Skontroluj či je to pre naše zariadenie
        if (userTokenFromTopic == _user_token && deviceTokenFromTopic == _device_token) {
            Serial.println("✅ Message is for this device - processing");
            
            // Parsuj JSON správu
            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, msg);
            
            if (error) {
                Serial.printf("❌ JSON parse failed: %s\n", error.c_str());
                return;     // Ak je JSON chybný, skonči
            }

            // Získaj hodnoty z JSONu
            const char* pin = doc["pin"];   // napr. "D0"
            double value = doc["value"];    // napr. 50
            
            Serial.printf("Processing command: %s = %.0f\n", pin, value);

            // Spracuj príkaz
            processCommand(pin, value);
            
        } else {
            // Správa je pre iné zariadenie - ignoruj
            Serial.println("❌ Message is for different device - ignoring");
        }
        return;
    }

    // Ak topic nie je v správnom formáte
    Serial.println("❌ Unknown topic format - ignoring");
}


// **************************************************************
//                PROCESS COMMAND + AUTO CONFIRM
// **************************************************************

/// Táto funkcia spracuje príkaz a zavolá callback funkciu
void ReceiveData::processCommand(const char* pin, double value) {

    // Prejdi cez všetky registrované callbacky
    for (auto& item : _callbacks) {

        // Skontroluj či tento callback je pre náš pin
        if (item.first == pin) {
            ControlCallback& cb = item.second;

            // Skontroluj či hodnota je v povolenom rozsahu
            if (value < cb.minValue || value > cb.maxValue) {
                Serial.printf("❌ Invalid value %s = %.2f (%.2f–%.2f)\n",
                    pin, value, cb.minValue, cb.maxValue);
                return;      // Hodnota je mimo rozsahu - skonči
            }

            // Hodnota je OK
            Serial.printf("✅ Execute %s = %.2f\n", pin, value);

            // Odošli potvrdenie späť do appky (rovnaká hodnota)
            Shyot.send(pin, value);
            Serial.printf("Potvrdenie prijatej hodnoty: %s = %.2f\n", pin, value);

            // Zavolaj callback
            cb.function(value, pin);

            return;     // Našli sme callback, skonči
        }
    }

    // Nenašli sme callback pre tento pin
    Serial.printf("⚠️ No handler for pin %s\n", pin);
}