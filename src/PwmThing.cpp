#include "PwmThing.h"
#include "driver/ledc.h" 

// Constructor with default parameters
PwmThing::PwmThing() : pinA(-1), pinB(-1), thingType(pwmOut), inverted(false), lastValue(0) {}

// Method to initialize the PwmThing
void PwmThing::begin(int pinA, int pinB, ThingType thingType, bool inverted, int servoMin, int servoZero, int servoMax) {
  this->pinA = pinA;
  this->pinB = pinB;
  this->thingType = thingType;
  this->inverted = inverted;
  this->lastValue = -1111;
  this->servoMin = servoMin;
  this->servoZero = servoZero;
  this->servoMax = servoMax;

  // Configure pins
  if (pinA >= 0) {
      pinMode(pinA, OUTPUT);
  }
  if (pinB >= 0) {
      pinMode(pinB, OUTPUT);
  }

  switch(thingType) {
      case pwmOut:
      case pwmOutGamma:
          // For simple PWM output, set frequency and resolution
          if (pinA >= 0) {
              analogWriteFrequency(pinA, 19531); // 20 kHz
              analogWriteResolution(pinA, 12);    // 12 bits (0-4095)
          }
          if (pinB >= 0) {
              analogWriteFrequency(pinB, 19531); // 20 kHz
              analogWriteResolution(pinB, 12);    // 12 bits (0-4095)
          }
          break;

      case halfBridge:
      case halfBridgeIdleHigh:
          // For half-bridge control, set frequency and resolution for both pins
          if ((pinA >= 0) && (pinB >= 0)) {
              analogWriteFrequency(pinA, 19531); // 20 kHz
              analogWriteResolution(pinA, 12);    // 12 bits (0-4095)
              analogWriteFrequency(pinB, 19531); // 20 kHz
              analogWriteResolution(pinB, 12);    // 12 bits (0-4095)
          } else {
              Serial.println("Half-bridge control requires two valid pins");
          }
          break;

      case servoMotor:
      case servoMotor0Stop:
          // For servo control, set frequency to 50Hz and resolution to 8 bits
          if (pinA >= 0) {
              analogWriteFrequency(pinA, 50);  // 50 Hz
              analogWriteResolution(pinA, 16); // 16 bits (0-65535) for better servo control
          }
          if (pinB >= 0) {
              analogWriteFrequency(pinB, 50);  // 50 Hz
              analogWriteResolution(pinB, 16); // 16 bits (0-65535) for better servo control
          }
          break;

      default:
          Serial.println("Invalid thingType");
          break;
  } 
  set(0); // Initialize to 0    
}

// Method to set the value
void PwmThing::set(int value) {
  if((pinA < 0) && (pinB < 0)) return; // No valid pins to control
  if(value == lastValue) return; // No change, skip
  value = constrain(value, -255, 255); // Constrain value to valid range
  lastValue = value;
  lastDuty = 0;
  int duty;
  if(logValues) Serial.printf("Pin %d = %d", pinA, value);

  switch (thingType) {
    case pwmOut:
    case pwmOutGamma: // same as pwmOut, but with gamma correction for LED brightness control
      if(inverted) value = 255 - value; // Invert value if needed
      value = constrain(value, 0, 255);
      if(thingType == pwmOutGamma) {
        // Apply gamma correction (using a simple approximation)
        float gamma = 2.2; // Common gamma value for LED brightness
        value = pow(value / 255.0, gamma) * (255.0 * 16.0);
      } else {
        value = value * 16; // Scale to 12-bit resolution (0-4095)
      }
      if (pinA >= 0) {
          analogWrite(pinA, value);
      }
      if (pinB >= 0) {
          analogWrite(pinB, value);
      }
      if(logValues) Serial.printf(", pwmOut %d\n", value);
      lastDuty = value;
      break;

    case halfBridge:
    case halfBridgeIdleHigh:
      if ((pinA >= 0) && (pinB >= 0)) {
        value = value * 16; // Scale to 12-bit resolution (0-4095)
        if(inverted) value = -value; // Invert direction if needed
        int phaseA = (value >= 0) ? value : 0; // Forward phase
        int phaseB = (value < 0) ? -value : 0; // Reverse phase
        if(thingType == halfBridgeIdleHigh) {
          phaseA = 4095 - phaseA;
          phaseB = 4095 - phaseB;
        }
        analogWrite(pinA, phaseA);
        analogWrite(pinB, phaseB);
        if(logValues) Serial.printf(", phaseA %d, phaseB %d\n", phaseA, phaseB);
        lastDuty = min(phaseA, phaseB);
      }
      break;   

    case servoMotor:
    case servoMotor0Stop:
      if(inverted) value = -value; // Invert direction if needed
      if(value >= 0) {
        duty = map(value, 0, 255, servoZero, servoMax); // Map to servo angle
      } else {
        duty = map(value, -255, 0, servoMin, servoZero); // Map to servo angle
      }
      if((thingType == servoMotor0Stop) && (value == 0)) duty = 0; // Turns servo off
      if (pinA >= 0) analogWrite(pinA, duty);
      if (pinB >= 0) analogWrite(pinB, duty);
      if(logValues) Serial.printf(", servo duty %d\n", duty);
      lastDuty = duty;
      break;

    default:
      Serial.println("Invalid thingType");
      break;
  }
}

void PwmThing::printInfo() {
  for(int i = 0; i < LEDC_CHANNEL_MAX; i++) {
    Serial.printf("LS Channel %u Duty: %u\n", i, ledc_get_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)i));
  }
  for(int i = 0; i < LEDC_TIMER_MAX; i++) {
    Serial.printf("LS Timer %u Freq: %u Hz\n", i, ledc_get_freq(LEDC_LOW_SPEED_MODE, (ledc_timer_t)i));
  } 
  for(int i = 0; i < LEDC_CHANNEL_MAX; i++) {
    Serial.printf("HS Channel %u Duty: %u\n", i, ledc_get_duty(LEDC_HIGH_SPEED_MODE, (ledc_channel_t)i));
  }
  for(int i = 0; i < LEDC_TIMER_MAX; i++) {
    Serial.printf("HS Timer %u Freq: %u Hz\n", i, ledc_get_freq(LEDC_HIGH_SPEED_MODE, (ledc_timer_t)i));
  } 
}