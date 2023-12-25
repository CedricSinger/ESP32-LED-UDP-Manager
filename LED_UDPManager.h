#ifndef LED_UDPMANAGER_H
#define LED_UDPMANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <AsyncUDP.h>
#include <IPAddress.h>
#include <util.h>

struct dataWrap{
    char data[256];
};

struct colorWrap{
    byte color1[3];
    byte color2[3];
};

class UDPManager {
    public:
        void startUp(IPAddress IP_ADDRESSES[10], IPAddress& GATEWAY, IPAddress& SUBNET, unsigned int UDP_PORT, char NETWORK_SSID[], char NETWORK_PASSWORD[]);
        void startUp(IPAddress& IP_ADDRESS, IPAddress& GATEWAY, IPAddress& SUBNET, unsigned int UDP_PORT, char NETWORK_SSID[], char NETWORK_PASSWORD[]);
        void sendOutPacket();
        void receivePacket(char* packetBuffer);
        LEDMode getMode();
        float getSpeed();
        int getBrightness();
        colorWrap getColors();
        LEDData getData();
        void setMode(LEDMode mode);
        void setSpeed(float speed);
        void setBrightness(int brightness);
        void setColor1(byte color[3]);
        void setColor2(byte color[3]);
        void setActive(int index, bool active);
        
        
    private:
        LEDData data;
        LEDData offData;
        bool is_receiver;
        IPAddress own_ip;
        IPAddress receivers[9];
        IPAddress gateway;
        IPAddress subnet;
        char* ssid;
        char* password;
        unsigned int udp_port;
        WiFiUDP udp;
        char packet_data[256];
        bool active_units[9];

        dataWrap dataToChars(LEDData data);
        LEDData charsToData(char* chars);
};

#endif