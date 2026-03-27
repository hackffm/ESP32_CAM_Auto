#ifndef PWMTHING_H
#define PWMTHING_H

#include <Arduino.h>

class PwmThing {
public:
    // Constructor with default parameters
    PwmThing();

    // Enum for thingType
    enum ThingType {
        pwmOut,       // Standard PWM, 20kHz, 8-bit resolution, 0...255
        pwmOutGamma,  // same as pwmOut, but with gamma correction for LED brightness control
        halfBridge,   // Half-bridge control, 20kHz, 8-bit resolution, -255...+255 (negative values reverse direction)
        halfBridgeIdleHigh, // as above, but both phases idle HIGH (often better for smoother motor running)
        servoMotor,    // Servo control, 50Hz, 14-bit resolution, -255...+255 (maps to 0.5ms...2.5ms pulse width)
        servoMotor0Stop, // as above, but with 0% duty cycle when value is 0
    };

    // Method to initialize the PwmThing
    void begin(int pinA, int pinB = -1, ThingType thingType = pwmOut, bool inverted = false);

    // Method to set the value
    void set(int value);

    // Method to get latest set value
    int get() { return lastValue; }

    void printInfo();

//private:
    int pinA;
    int pinB;
    ThingType thingType;
    bool inverted;
    int lastValue; 
    int servoMin = 768;
    int servoZero = 4760;
    int servoMax = 9544;

    bool logValues = false;
    
};

#endif // PWMTHING_H
