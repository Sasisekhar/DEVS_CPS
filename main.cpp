#include "MQTTDriver.h"
#include "ThisThread.h"
#include "us_ticker_api.h"
// main() runs in its own thread in the OS

DigitalOut led1(D13);
DigitalIn button(BUTTON1);

int main() {
    led1 = false;

    MQTTDriver client;
    client.init();
    client.connect("ARSLAB_H743ZI2");
    client.subscribe("ARSLAB/ping/req");

    for(int i = 0; i < 4; i++) {
        led1 = true;
        ThisThread::sleep_for(100ms);

        led1 = false;
        ThisThread::sleep_for(100ms);
    }

    uint64_t startTime = 0;
    char topic[128];
    char message[128];

    while (true) {

        if((us_ticker_read()/1000) -  startTime > 30000) {
            client.ping();
            startTime = us_ticker_read()/1000;
        }

        if(client.receive_response(topic, message)) {
            if(!strcmp(topic, (char*) "ARSLAB/ping/req")) {
                client.publish("ARSLAB/ping/resp", message);
                if(!strcmp(message, (char*) "1")) {
                    led1 = true;
                } else if (!strcmp(message, (char*) "0")){
                    led1 = false;
                }
            } 
        }

        if(button) {
            break;
        }
    }

    client.disconnect();

    return 0;
}
