#ifndef UTIL_H
#define UTIL_H

#include <Arduino.h>

enum LEDMode {OFF, ONECOLOR, WAVE, PULSE, RAINBOW};

struct LEDData{
    LEDMode led_mode;
    float periodic_speed;
    int brightness;
    byte colors[2][3];
};

#endif