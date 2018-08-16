
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

#endif    // __configs_h__
