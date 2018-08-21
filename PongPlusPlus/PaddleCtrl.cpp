/*
 * PaddleCtrl.cpp
 *
 *  Created on: Aug 16, 2018
 *      Author: athiessen
 */

#include <Arduino.h>

#include "Configs.h"
#include "IntHandler.h"
#include "PaddleCtrl.h"
#include "PaddleStatus.h"
#include "ScheduledInterval.h"
#include "Timing.h"


#define BUTTON_DEBOUNCE_TIME_MS     (50)


PaddleCtrl::PaddleCtrl(PaddleConf&      config,
                       PaddleStatus&   _status):
   ScheduledInterval(PADDLE_CHECK_INTERVAL),
   DT(config.DT),
   CLK(config.CLK),
   buttonPin(config.buttonPin),
   paddleStatus(_status),
   hwStatus(_status),
   previousButtonState(false),
   updateCount(0),
   msCount(0)
{
   RegisterInterrupt(DT, this);
   RegisterInterrupt(CLK, this);
}


void HandleInterrup(uint8_t pin)
{

}


void PaddleCtrl::Update()
{
   // Check the rotary encoder and button every time we're called
   CheckRotaryEncoder();
   CheckButton();

   // Periodically update the external paddle status base on our current state
   if(++updateCount >= PADDLE_UPDATE_INTERVAL)
   {
      UpdatePaddleStatus();
      updateCount = 0;
   }
}


void PaddleCtrl::UpdatePaddleStatus()
{
   // If the value has changed, then increment the status accordingly
   // and reset value
   if(hwStatus.position != 0)
   {
      paddleStatus.Increment(hwStatus.position);
      hwStatus.position = 0;
   }

   paddleStatus.buttonPressed = hwStatus.buttonPressed;
   paddleStatus.buttonTime = hwStatus.buttonTime;
}


void PaddleCtrl::CheckRotaryEncoder()
{
   // TODO: This is where we need to update internalStatus.position from the Rotary Encoder value
   //       This can be polled or interrupt driven...
   //       except, if we do interrupts, we will need to do a little fancy C++ foot-work
}


void PaddleCtrl::CheckButton()
{
   // TODO: Does this debounce algorithm work properly?

   // Increment the amount of time in ms the button has been in its current state
   if(++msCount >= PADDLE_BUTTON_INCR_INTERVAL)
   {
      hwStatus.buttonTime++;
      msCount = 0;
   }

   if(hwStatus.buttonTime >= BUTTON_DEBOUNCE_TIME_MS)
   {
      bool currentStatus = (digitalRead(buttonPin) == HIGH) ? true : false;

      if(currentStatus == previousButtonState)
      {
         if(currentStatus != hwStatus.buttonPressed)
         {
            hwStatus.buttonTime = 0;
            hwStatus.buttonPressed = currentStatus;
         }
      }

      previousButtonState = currentStatus;
   }
}
