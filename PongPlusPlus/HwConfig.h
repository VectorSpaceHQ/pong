/*
 * HwConfig.h
 *
 *  Created on: Aug 17, 2018
 *      Author: athiessen
 */

#ifndef __hw_config_h__
#define __hw_config_h__

#include <Arduino.h>

#include "Configs.h"


struct HwConfig
{

public:
   LaserConf      leftPlayerLaser;
   LaserConf      rightPlayerLaser;
   LaserConf      ballLaser;

   PaddleConf     leftPaddle;
   PaddleConf     rightPaddle;

   HwConfig():
      leftPlayerLaser(
         3,    // Gimble X Pin
         5,    // Gimble Y Pin
         A2 ), // Laser Pin

     rightPlayerLaser(
         6,    // Gimble X Pin
         9,    // Gimble Y Pin
         A3 ), // Laser Pin

      ballLaser(
         10,   // Gimble X Pin
         11,   // Gimble Y Pin
         A4 ), // Laser Pin

      leftPaddle(
         7,    // DT
         8,    // CLK
         A0 ), // Button Pin

      rightPaddle(
         2,    // DT
         4,    // CLK
         A1 )  // Button Pin
   {
   }

};



#endif   // __hw_config_h__
