#include "MQTTDriver.h"
#include <cstring>
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

    if(client.subscribe("ARSLAB/Data/CO")) {
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
    char message[128];

    int temp, hum, co;

    while (true) {

        if((us_ticker_read()/1000) - client.lastActivity() > 30000) {
            client.ping();
        }

        if(client.receive_response(topic, message)) {

            if(!strcmp(topic, (char*) "ARSLAB/Data/Temp")) {
                string tmp = "";
                temp = 0;

                // printf("%d\r\n", strlen(message));

                for(int i = 0; i < strlen(message); i++) {
                    // printf("%s\r\n", tmp.c_str());
                    if(message[i] != ',') {
                        tmp += message[i];
                    } else if(message[i] == ',') {
                        temp += stoi(tmp);
                        tmp = "";
                    }
                }

                temp /= 3;

            }
            if(!strcmp(topic, (char*) "ARSLAB/Data/Hum")) {

                string tmp = "";
                hum = 0;

                for(int i = 0; i < strlen(message); i++) {
                    if(message[i] != ',') {
                        tmp += message[i];
                    } else if(message[i] == ',') {
                        hum += stoi(tmp);
                        tmp = "";
                    }
                }

                hum /= 3;
                
            }
            
            if(!strcmp(topic, (char*) "ARSLAB/Data/CO")) {

                string tmp = "";
                co = 0;

                for(int i = 0; i < strlen(message); i++) {
                    if(message[i] != ',') {
                        tmp += message[i];
                    } else if(message[i] == ',') {
                        co += stoi(tmp);
                        tmp = "";
                    }
                }

                co /= 3;
                
            }

            char buff[64];
            sprintf(buff, "{\"Temp\":%d, \"Hum\":%d, \"CO\":%d}", temp, hum, co);

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
