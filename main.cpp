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

    if(client.subscribe("ARSLAB/Control/Light")) {
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

    while (true) {

        if((us_ticker_read()/1000) -  startTime > 30000) {
            //client.ping();
            startTime = us_ticker_read()/1000;
        }

        if(client.receive_response(topic,(char*) message.c_str())) {
            if(!strcmp(topic, (char*) "ARSLAB/Contorl/Light")) {
                led1 = ~led1;
            }
        }

        if(button) {
            break;
        }
    }

    client.disconnect();
    printf("Done!\r\n");

    return 0;
}
