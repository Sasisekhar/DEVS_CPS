#include "MQTTDriver.h"
#include "ThisThread.h"
// main() runs in its own thread in the OS

DigitalOut led1(D12);
DigitalOut led2(D13);
DigitalIn button(BUTTON1);

int main() {
    led1 = false;
    led2 = false;

    MQTTDriver client;
    client.init();

    client.connect("ARSLAB");

    client.subscribe("ARSLAB/Control/AC");

    client.subscribe("ARSLAB/Control/Door");

    client.subscribe("ARSLAB/Control/Light");

    for(int i = 0; i < 4; i++) {
        led1 = true;
        led2 = true;
        ThisThread::sleep_for(100ms);

        led1 = false;
        led2 = false;
        ThisThread::sleep_for(100ms);
    }

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
            ThisThread::sleep_for(1000ms);
            if(!button){
                break;
            }
        }
    }

    printf("Done\r\n");

    client.disconnect();

    return 0;
}
