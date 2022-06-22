#include "mbed.h"
#include "MQTTDriver.h"
// main() runs in its own thread in the OS

namespace arduino{
    uint64_t millis() {
        return (us_ticker_read()/1000);
    }
}

ESP8266Interface WiFi(D1, D0);
MQTTclient client;

int main() {

    MQTTDriver client(  D1,
                        D0,
                        (char*) "Sx3K",
                        (char*) "golikuttan7577",
                        NSAPI_SECURITY_WPA_WPA2,
                        (char*) "broker.hivemq.com",
                        1883,
                        (char*) "ARSLAB",
                        NULL,
                        NULL );

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

            client.publish((char*) "ARSLAB/Data/Raw", buff);
            startTime = arduino::millis();
        }

        if(client.receive_response(topic, message)) {
            printf("Message: %s received on topic: %s\n", message, topic);
        }
    }

    WiFi.disconnect();

    return 0;
}