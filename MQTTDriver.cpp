#include "MQTTDriver.h"
// main() runs in its own thread in the OS

bool MQTTDriver::init(MQTTclient* CLIENT) {

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

    MQTTclient client(&WiFi, address);

    if(client.connect("ARSLAB")) {
        printf("Connection Successful\n");
    }

    if(client.subscribe("ARSLAB/Control/AC")) {
        printf("Subscription successful\n");
    }

    if(client.subscribe("ARSLAB/Control/Door")) {
        printf("Subscription successful\n");
    }

    if(client.subscribe("ARSLAB/Control/Light")) {
        printf("Subscription successful\n");
    }

    CLIENT = &client;

    return true;
}
