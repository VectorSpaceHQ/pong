
#ifndef __configs_h__
#define __configs_h__

#include <stdint.h>

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

struct PaddleConf
{
public:
   uint8_t  upPin;
   uint8_t  downPin;
   uint8_t  buttonPin;

   PaddleConf(uint8_t _upPin, uint8_t _downPin, uint8_t _buttonPin):
      upPin(_upPin),
      downPin(_downPin),
      buttonPin(_buttonPin)
   {
   }
};

#endif    // __configs_h__
