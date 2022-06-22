#include "DigitalIn.h"
#include "DigitalOut.h"
#include "PinNameAliases.h"
#include "SocketAddress.h"
#include "TCPSocket.h"
#include "ThisThread.h"
#include "WiFiInterface.h"
#include "mbed.h"
#include "ESP8266Interface.h"
#include "MQTT.h"
#include <cstring>
#include "nsapi_types.h"


class MQTTDriver {
    private:
        ESP8266Interface    *_ESPclient;
        MQTTclient          _MQTTclient;

    public:
        MQTTDriver( PinName             Tx,
                    PinName             Rx,
                    char*               SSID,
                    char*               PSWD,
                    nsapi_security      security,
                    char*               Broker,
                    int                 Port,
                    char*               ClientID,
                    char*               Username,
                    char*               Pwd);
        ~MQTTDriver();

        bool publish(char*, char*);
        bool subscribe(char*);
        bool receive_response(char*, char*);
};