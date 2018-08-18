
#ifndef __configs_h__
#define __configs_h__

#include <stdint.h>


/***
 * The LaserConf configuration structure holds configuration information for a laser
 * including gimbal and laser control
 */
struct LaserConf
{
public:
   uint8_t xPin;
   uint8_t yPin;
   uint8_t laserPin;

   LaserConf(uint8_t _xPin, uint8_t _yPin, uint8_t _laserPin):
      xPin(_xPin),
      yPin(_yPin),
      laserPin(_laserPin)
   {
   }
};


/***
 * The PaddConf configuration structure holds configuration information for a paddle
 * controller (user input), including rotary encoder and push button
 */
struct PaddleConf
{
public:
   uint8_t  pinA;          // Rotary pin A
   uint8_t  pinB;          // Rotary pin B
   uint8_t  buttonPin;

   PaddleConf(uint8_t _pinA, uint8_t _pinB, uint8_t _buttonPin):
      pinA(_pinA),
      pinB(_pinB),
      buttonPin(_buttonPin)
   {
   }
};

#endif    // __configs_h__
