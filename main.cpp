#include "MQTTDriver.h"
#include <string>
// main() runs in its own thread in the OS

DigitalOut led1(D13);
DigitalIn button(BUTTON1);

int main() {
    led1 = false;

    MQTTDriver client;
    client.init();
    printf("Connecting to the broker...\n\r");
    if(client.connect("ARSLAB_Processor")) {
        printf("Connected!\n\r");
    }

    printf("Subscribing to the topics:-\n\r");

    if(client.subscribe("ARSLAB/Data/Temp")) {
        printf("Subscription Successful!\n\r");
    }

    if(client.subscribe("ARSLAB/Data/Hum")) {
        printf("Subscription Successful!\n\r");
    }

    if(client.subscribe("ARSLAB/Data/CO2")) {
        printf("Subscription Successful!\n\r");
    }

    for(int i = 0; i < 4; i++) {
        led1 = true;
        ThisThread::sleep_for(100ms);

        led1 = false;
        ThisThread::sleep_for(100ms);
    }

    uint64_t startTime = 0;
    char topic[128];
    string message;

    int temp, hum, co2;

    while (true) {

        if((us_ticker_read()/1000) -  startTime > 30000) {
            client.ping();
            startTime = us_ticker_read()/1000;
        }

        if(client.receive_response(topic,(char*) message.c_str())) {
            if(!strcmp(topic, (char*) "ARSLAB/Data/Temp")) {
                string tmp = " ";
                temp = 0;

                for(int i = 0; i < message.length(); i++) {
                    if(message[i] != ',') {
                        tmp += message[i];
                    } else if(message[i] == ',') {
                        temp += stoi(tmp);
                    }
                }

                temp /= 3;

            } else if(!strcmp(topic, (char*) "ARSLAB/Data/Hum")) {

                string tmp = " ";
                hum = 0;

                for(int i = 0; i < message.length(); i++) {
                    if(message[i] != ',') {
                        tmp += message[i];
                    } else if(message[i] == ',') {
                        hum += stoi(tmp);
                    }
                }

                hum /= 3;
                
            } else if(!strcmp(topic, (char*) "ARSLAB/Data/CO2")) {

                string tmp = " ";
                co2 = 0;

                for(int i = 0; i < message.length(); i++) {
                    if(message[i] != ',') {
                        tmp += message[i];
                    } else if(message[i] == ',') {
                        co2 += stoi(tmp);
                    }
                }

                co2 /= 3;
                
            }

            char buff[64];
            sprintf(buff, "{\"temp\":%d, \"hum\":%d, \"CO2\":%d}", temp, hum, co2);

            client.publish("ARSLAB/Data/Fused", buff);
        }

        if(button) {
            break;
        }
    }

    client.disconnect();
    printf("Done!\r\n");

    return 0;
}
