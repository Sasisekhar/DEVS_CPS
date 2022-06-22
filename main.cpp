#include "MQTTDriver.h"
// main() runs in its own thread in the OS

DigitalOut led1(D12);
DigitalOut led2(D13);

int main() {
    led1 = false;
    led2 = false;

    DigitalIn button(BUTTON1);

    MQTTDriver client;

    uint64_t startTime = 0;
    char topic[128];
    char message[128];

    while (true) {

        if((us_ticker_read()/1000) -  startTime > 1000) {
            int temp = rand()%50;
            int hum = rand()%100;
            int co2 = rand()%5000;

            char buff[128];

            sprintf(buff, "{\"Temp\":%d, \"Hum\":%d, \"CO2\":%d}", temp, hum, co2);

            client.publish("ARSLAB/Data/Raw", buff);
            startTime = us_ticker_read()/1000;
        }

        if(client.receive_response(topic, message)) {
            //printf("Message: %s received on topic: %s\n", message, topic);

            if(!strcmp(topic, (char*) "ARSLAB/Control/Door")) {
                if(!strcmp(message, (char*) "1")) {
                    led1 = true;
                } else {
                    led1 = false;
                }
            } else if (!strcmp(topic, (char*) "ARSLAB/Control/Light")) {
                if(!strcmp(message, (char*) "1")) {
                    led2 = true;
                } else {
                    led2 = false;
                }
            }
        }

        if(!button) {
            client.ping();
            ThisThread::sleep_for(500ms);
            if(!button){
                break;
            }
        }
    }

    return 0;
}
