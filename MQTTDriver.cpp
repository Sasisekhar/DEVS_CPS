#include "MQTTDriver.h"

MQTTDriver::MQTTDriver( PinName             Tx,
                        PinName             Rx,
                        char*               SSID,
                        char*               PSWD,
                        nsapi_security      security,
                        char*               Broker,
                        int                 Port,
                        char*               ClientID,
                        char*               Username,
                        char*               Pwd) {

    ESP8266Interface ESPclient(Tx, Rx);    
    _ESPclient = &ESPclient;

    _ESPclient->set_credentials(SSID, PSWD, security);

    _ESPclient->connect();

    printf("Connecting");

    while(_ESPclient->get_connection_status() == NSAPI_STATUS_CONNECTING){
        printf(".");
        ThisThread::sleep_for(500ms);
    }

    if(_ESPclient->get_connection_status() == NSAPI_STATUS_GLOBAL_UP) {
        printf("\r\nConnected!\r\n");
    }

    SocketAddress address;
    _ESPclient->gethostbyname(Broker, &address);
    address.set_port(Port);

    _MQTTclient.initializeClass(_ESPclient, address);

    if(_MQTTclient.connect(ClientID, Username, Pwd)) {
        printf("Connection Successful\n");
    }

}

bool MQTTDriver::publish(char* topic, char* payload) {
    return _MQTTclient.publish(topic, payload);
}

bool MQTTDriver::subscribe(char* topic) {
    return _MQTTclient.subscribe(topic);
}

bool MQTTDriver::receive_response(char* topic, char* message){
    return _MQTTclient.receive_response(topic, message);
}

MQTTDriver::~MQTTDriver(){
    _ESPclient->disconnect();
}