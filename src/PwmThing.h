#ifndef PWMTHING_H
#define PWMTHING_H

#include <Arduino.h>

class PwmThing {
public:
    // Constructor with default parameters
    PwmThing();

    static constexpr int PWMHIGHFREQ = 19531; // 19531;

    // Enum for thingType
    enum ThingType {
        pwmOut = 0,       // Standard PWM, 20kHz, 8-bit resolution, 0...255
        pwmOutGamma = 1,  // same as pwmOut, but with gamma correction for LED brightness control
        halfBridge = 2,   // Half-bridge control, 20kHz, 8-bit resolution, -255...+255 (negative values reverse direction)
        halfBridgeIdleHigh = 3, // as above, but both phases idle HIGH (often better for smoother motor running)
        servoMotor = 4,    // Servo control, 50Hz, 14-bit resolution, -255...+255 (maps to 0.5ms...2.5ms pulse width)
        servoMotor0Stop = 5, // as above, but with 0% duty cycle when value is 0
        thingTypeCount
    };

    // Method to initialize the PwmThing
    void begin(int pinA, int pinB = -1, ThingType thingType = pwmOut, bool inverted = false, 
      int servoMin = 768, int servoZero = 4760, int servoMax = 9544);

    // Method to set to lowest power
    void end();  

    // Method to set the value (0...255 for pwmOut, -255...+255 for halfBridge and servoMotor)
    void set(int value, bool clearAnimation = true);

    // Method to get latest set value
    int get() { return lastValue; }

    int getDuty() { return lastDuty; }

    void printInfo();

    void startAnimation(int animationType, int speed = 2000, int lowValue = 0, int highValue = 255) {
      animationLowValue = lowValue; animationHighValue = highValue;
      animationSpeed = speed; this->animationType = animationType;
    }

    void doAnimation();

//private:
    int pinA;
    int pinB;
    ThingType thingType;
    bool inverted;
    
    int servoMin = 768;
    int servoZero = 4760;
    int servoMax = 9544;

    bool logValues = false;
    int lastValue;
    int lastDuty;

    int animationType = 0;
    int animationLowValue = 0;
    int animationHighValue = 255;
    int animationSpeed = 1000; // in milliseconds
};

#endif // PWMTHING_H
