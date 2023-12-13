#include "LED_UDPManager.h"
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <AsyncUDP.h>
#include <IPAddress.h>



void UDPManager::startUp(bool IS_RECEIVER, IPAddress IP_ADDRESSES[10], IPAddress& GATEWAY, IPAddress& SUBNET, unsigned int UDP_PORT, char NETWORK_SSID[], char NETWORK_PASSWORD[]){
    is_receiver = IS_RECEIVER;

    data.led_mode = 0;
    data.periodic_speed = 100;
    data.brightness = 100;
    data.colors[0][0] = 0;
    data.colors[0][1] = 0;
    data.colors[0][2] = 0;
    data.colors[1][0] = 0;
    data.colors[1][1] = 0;
    data.colors[1][2] = 0;

    offData.led_mode = 0;
    offData.periodic_speed = 100;
    offData.brightness = 100;
    offData.colors[0][0] = 0;
    offData.colors[0][1] = 0;
    offData.colors[0][2] = 0;
    offData.colors[1][0] = 0;
    offData.colors[1][1] = 0;
    offData.colors[1][2] = 0;
    
    gateway = GATEWAY;
    subnet = SUBNET;
    ssid = NETWORK_SSID;
    password = NETWORK_PASSWORD;
    udp_port = UDP_PORT;



    if(!IS_RECEIVER){
        for(int i = 0; i < 10; i++){
            active_units[i] = true;
        }

        own_ip = IP_ADDRESSES[0];
        for(int i = 1; i < 10; i++){
            if(IP_ADDRESSES[i] == IPAddress(0,0,0,0)){
                active_units[i - 1] = false;
            }
            else{
                receivers[i - 1] = IP_ADDRESSES[i];
            }
        }
    }
    else{
        own_ip = IP_ADDRESSES[0];
    }


    
    
    WiFi.config(own_ip, gateway, subnet);
    WiFi.mode(WIFI_STA);
    WiFi.begin(NETWORK_SSID, NETWORK_PASSWORD);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.print("Connected, IP address: ");
    Serial.println(WiFi.localIP());
}

void UDPManager::sendOutPacket(){
    for(int i = 0; i < 10; i++){
        if(active_units[i]){    
            udp.beginPacket(receivers[i],udp_port);
            for(int i = 0; i < sizeof(packet_data); i++){
                udp.write(packet_data[i]);
            }
            udp.endPacket();
        }
    }
}

void UDPManager::receivePacket(char* packetBuffer){
    int size = udp.parsePacket();
    if(size != 0){
        int length = udp.read(packetBuffer, 255);
        if(length > 0){
            packetBuffer[length] = 0;
        }
        data = charsToData(packetBuffer);
    }
}



char UDPManager::getMode(){
    return data.led_mode;
}

float UDPManager::getSpeed(){
    return data.periodic_speed;
}

int UDPManager::getBrightness(){
    return data.brightness;
}

colorWrap UDPManager::getColors(){
    struct colorWrap result;
    result.color1[0] = data.colors[0][0];
    result.color1[1] = data.colors[0][1];
    result.color1[2] = data.colors[0][2];
    result.color2[0] = data.colors[1][0];
    result.color2[1] = data.colors[1][1];
    result.color2[2] = data.colors[1][2];
    return result;
}

void UDPManager::setMode(char mode){
    data.led_mode = mode;
}

void UDPManager::setSpeed(float speed){
    data.periodic_speed = speed;
}

void UDPManager::setBrightness(int brightness){
    data.brightness = brightness;
}

void UDPManager::setColor1(byte color[3]){
    data.colors[0][0] = color[0];
    data.colors[0][1] = color[1];
    data.colors[0][2] = color[2];
}
        
void UDPManager::setColor2(byte color[3]){
    data.colors[1][0] = color[0];
    data.colors[1][1] = color[1];
    data.colors[1][2] = color[2];
}

void UDPManager::setActive(int index, bool active){
    active_units[index - 1] = active;
}


dataWrap UDPManager::dataToChars(LEDData data){
    char result[256];
    String temp;
    for(int i = 0; i < 3; i++){
        temp = String(data.colors[0][i]);
        result[3 * i] = temp.charAt(0);
        result[3 * i + 1] = temp.charAt(1);
        result[3 * i + 2] = temp.charAt(2);
        packet_data[3 * i] = temp.charAt(0);
        packet_data[3 * i + 1] = temp.charAt(1);
        packet_data[3 * i + 2] = temp.charAt(2);
    }

    for(int i = 0; i < 3; i++){
        temp = String(data.colors[1][i]);
        result[3 * i + 9] = temp.charAt(0);
        result[3 * i + 10] = temp.charAt(1);
        result[3 * i + 11] = temp.charAt(2);
        packet_data[3 * i + 9] = temp.charAt(0);
        packet_data[3 * i + 10] = temp.charAt(1);
        packet_data[3 * i + 11] = temp.charAt(2);
    }

    temp = data.led_mode;
    result[18] = temp.charAt(0);

    for(int i = 0; i < 3; i++){
        temp = String(data.periodic_speed);
        result[3 * i + 19] = temp.charAt(0);
        result[3 * i + 20] = temp.charAt(1);
        result[3 * i + 21] = temp.charAt(2);
        packet_data[3 * i + 19] = temp.charAt(0);
        packet_data[3 * i + 20] = temp.charAt(1);
        packet_data[3 * i + 21] = temp.charAt(2);
    }

    for(int i = 0; i < 3; i++){
        temp = String(data.brightness);
        result[3 * i + 22] = temp.charAt(0);
        result[3 * i + 23] = temp.charAt(1);
        result[3 * i + 24] = temp.charAt(2);
        packet_data[3 * i + 22] = temp.charAt(0);
        packet_data[3 * i + 23] = temp.charAt(1);
        packet_data[3 * i + 24] = temp.charAt(2);
    }

    struct dataWrap resultWrap;
    for(int i = 0; i < 256; i++){
        resultWrap.data[i] = result[i];
    }
    return resultWrap;
}

LEDData UDPManager::charsToData(char* chars){
    LEDData result;
    String temp;
    for(int i = 0; i < 3; i++){
        temp = "000";
        for(int j = 0; j < 3; j++){
            temp.setCharAt(i, chars[j + 3 * i]);
        }
        result.colors[0][i] = temp.toInt();
    }
    for(int i = 0; i < 3; i++){
        temp = "000";
        for(int j = 0; j < 3; j++){
            temp.setCharAt(i, chars[j + 3 * i + 9]);
        }
        result.colors[1][i] = temp.toInt();
    }

    result.led_mode = chars[18];

    temp = "000";
    for(int i = 19; i < 22; i++){
        temp.setCharAt(i, chars[i]);
    }
    result.periodic_speed = temp.toInt();

    temp = "000";
    for(int i = 22; i < 25; i++){
        temp.setCharAt(i, chars[i]);
    }
    result.brightness = temp.toInt();

    return result;
}