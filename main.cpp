#include "MQTTDriver.h"
#include "ThisThread.h"
// main() runs in its own thread in the OS

DigitalOut led1(D13);
DigitalIn button(BUTTON1);
DigitalOut trig(D12);

int main() {
    led1 = false;

    MQTTDriver client;
    client.init();
    client.connect("ARSLAB_4S5IIOT01");
    
    client.subscribe("ARSLAB/ping/resp");

    for(int i = 0; i < 4; i++) {
        led1 = true;
        ThisThread::sleep_for(100ms);

        led1 = false;
        ThisThread::sleep_for(100ms);
    }

    uint64_t startTime = 0;
    char topic[128];
    char message[128];
    bool currState;

    while (true) {

        if((us_ticker_read()/1000) -  startTime > 30000) {
            client.ping();
            startTime = us_ticker_read()/1000;
        }

        if(client.receive_response(topic, message)) {
            if(!strcmp(topic, (char*) "ARSLAB/ping/resp")) {
                if(!strcmp(message, (char*) "1")) {
                    led1 = true;
                } else if (!strcmp(message, (char*) "0")){
                    led1 = false;
                }
            } 
        }

        trig = button;
        if(!button) {

            if(!currState) {
                client.publish("ARSLAB/ping/req",(char*) "1");
                currState = true;
            }

            while (true) {
                if(client.receive_response(topic, message)) {
                    if(!strcmp(topic, (char*) "ARSLAB/ping/resp")) {
                        if(!strcmp(message, (char*) "1")) {
                            led1 = true;
                        } else if (!strcmp(message, (char*) "0")){
                            led1 = false;
                        }
                    }

                    break;
                }
            }

            //client.ping();
            ThisThread::sleep_for(1000ms);
            if(!button){
                break;
            }
        } else {
            if(currState) {
                client.publish("ARSLAB/ping/req",(char*) "0");
                currState = false;
            }
        }
    }

    printf("Done\r\n");

    client.disconnect();

    return 0;
}
