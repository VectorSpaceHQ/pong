/*
 * PaddleDriver.h
 *
 *  Created on: Aug 16, 2018
 *      Author: athiessen
 */

#ifndef __paddle_driver_h__
#define __paddle_driver_h__

#include <stdint.h>

#include "Configs.h"
// #include "IntHandler.h"
#include "PaddleStatus.h"
#include "ScheduledInterval.h"


class PaddleDriver: public ScheduledInterval
{
public:
   PaddleDriver(PaddleConf&     config,
               PaddleStatus&   _status);

private:
   uint8_t              DT;          // Rotary Encoder DT Pin
   uint8_t              CLK;          // Rotary Encoder CLK Pin
   uint8_t              buttonPin;
   uint8_t              oldState;
   PaddleStatus&        paddleStatus;
   PaddleStatus         hwStatus;
   bool                 previousButtonState;
   uint32_t             updateCount;
   uint32_t             msCount;

   void Update();
   void UpdatePaddleStatus();
   void CheckButton();
   void CheckRotaryEncoder();
};


#endif   // __paddle_driver_h__
