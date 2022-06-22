#include "MQTTDriver.h"

namespace arduino{
    uint64_t millis() {
        return (us_ticker_read()/1000);
    }
}

bool MQTTDriver::init() {

    ESP8266Interface WiFi(D1, D0);

    WiFi.set_credentials((const char*)"Sx3K", (const char*)"golikuttan7577", NSAPI_SECURITY_WPA_WPA2);
    // WiFi.set_credentials((const char*)"ARS-LAB", (const char*)"3928DC6C25", NSAPI_SECURITY_WEP);

    WiFi.connect();
    printf("Connecting");
    while(WiFi.get_connection_status() == NSAPI_STATUS_CONNECTING){
        printf(".");
        ThisThread::sleep_for(500ms);
    }

    if(WiFi.get_connection_status() == NSAPI_STATUS_GLOBAL_UP) {
        printf("\r\nConnected!\r\n");
    }

    SocketAddress address;
    WiFi.gethostbyname("broker.hivemq.com", &address);
    // address.set_ip_address("134.117.52.253\0");  //My laptop
    // address.set_ip_address("134.117.52.231\0");     //My workstation
    address.set_port(1883);

    _client.initializeClass(&WiFi, address);

    if(_client.connect("ARSLAB")) {
        printf("Connection Successful\n");
    }

    if(_client.subscribe("ARSLAB/Control/AC")) {
        printf("Subscription successful\n");
    }

    if(_client.subscribe("ARSLAB/Control/Door")) {
        printf("Subscription successful\n");
    }

    if(_client.subscribe("ARSLAB/Control/Light")) {
        printf("Subscription successful\n");
    }

    uint64_t startTime = 0;
    char topic[128];
    char message[128];

    while (true) {

        if(arduino::millis() -  startTime > 1000) {
            int temp = rand()%50;
            int hum = rand()%100;
            int co2 = rand()%5000;

            char buff[128];

            sprintf(buff, "{\"Temp\":%d, \"Hum\":%d, \"CO2\":%d}", temp, hum, co2);

            _client.publish("ARSLAB/Data/Raw", buff);
            startTime = arduino::millis();
        }

        if(_client.receive_response(topic, message)) {
            printf("Message: %s received on topic: %s\n", message, topic);
        }
    }

    WiFi.disconnect();

    return true;
}

bool MQTTDriver::publish(const char* topic, char* message) {
    return _client.publish(topic, message);
}

bool MQTTDriver::receive_response(char* topic, char* message) {
    return _client.receive_response(topic, message);
}

bool MQTTDriver::subscribe(char* topic) {
    return _client.subscribe(topic);
}

bool MQTTDriver::ping() {
    return _client.ping();
}
