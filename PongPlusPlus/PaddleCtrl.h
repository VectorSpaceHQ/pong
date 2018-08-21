/*
 * PaddleCtrl.h
 *
 *  Created on: Aug 16, 2018
 *      Author: athiessen
 */

#ifndef __paddle_ctrl_h__
#define __paddle_ctrl_h__

#include <stdint.h>

#include "Configs.h"
#include "IntHandler.h"
#include "PaddleStatus.h"
#include "ScheduledInterval.h"


class PaddleCtrl: public ScheduledInterval, IntHandler
{
public:
   PaddleCtrl(PaddleConf&     config,
              PaddleStatus&   _status);

   void HandleInterrupt(uint8_t pin);

private:
   uint8_t              DT;          // Rotary Encoder DT Pin
   uint8_t              CLK;          // Rotary Encoder CLK Pin
   uint8_t              buttonPin;
   PaddleStatus&        paddleStatus;
   PaddleStatus         hwStatus;
   bool                 previousButtonState;
   uint32_t             updateCount;
   uint32_t             msCount;

   void Update();
   void UpdatePaddleStatus();
   void CheckRotaryEncoder();
   void CheckButton();
};



#endif   // __paddle_ctrl_h__
