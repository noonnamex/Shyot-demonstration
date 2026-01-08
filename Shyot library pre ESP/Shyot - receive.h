// **************************************************************
//                          RECIEVE DATA
// **************************************************************
#include <vector>      
#include <functional>

/// štruktúra, ktorá uchováva callback funkcie pre piny
struct ControlCallback {
    std::function<void(double value, const char* pin)> function;
    double minValue;                                                // min povolená hodnota
    double maxValue;                                                // max povolená hodnota
};

/// trieda, ktorá sa stará o prijímanie MQTT dát z aplikácie
class ReceiveData {
public:

    // umožňuje triede SendData pristupovať k private členom ReceiveData
    friend class SendData;

    // registrácia vlastného pinu
    void registerCustom(const char* pin, double min, double max, std::function<void(double, const char*)> callback);

    // funkcia, ktoré volá MQTT klient hneď po prijatí správy
    void handleMessage(char* topic, byte* payload, unsigned int length);


private:
    // tokeny pre kontrolu komu správa patrí
    const char* _user_token;
    const char* _device_token;

    // zoznam všetkých registrovaných pinov
    std::vector<std::pair<String, ControlCallback>> _callbacks;

    // funkcia zavolá správny callback podľa pinu
    void processCommand(const char* pin, double value);
};

// deklarácia objektu Receiver
extern ReceiveData Receiver;