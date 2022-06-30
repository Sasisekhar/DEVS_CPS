#include "MQTTDriver.h"
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
    char message[128];

    while (true) {

        if((us_ticker_read()/1000) -  startTime > 5000) {
            char buff[128];

            sprintf(buff, "{\"Temp\": {\"Temp1\": %d, \"Temp2\": %d, \"Temp3\": %d}, \"Hum\": {\"Hum1\": %d, \"Hum2\": %d, \"Hum3\": %d}, \"CO\": {\"CO1\": %d, \"CO2\": %d, \"CO3\": %d}}", rand()%50, rand()%50, rand()%50, rand()%100, rand()%100, rand()%100, rand()%5000, rand()%5000,rand()%5000);

            client.publish("ARSLAB/Data/Raw", buff);
            startTime = us_ticker_read()/1000;
        }

        if(client.receive_response(topic, message)) {
            if(!strcmp(topic, (char*) "ARSLAB/Control/Light")) {
                led1 = !led1;
            }
        }

        if(!button) {
            break;
        }
    }

    client.disconnect();
    printf("Done!\r\n");

    return 0;
}
