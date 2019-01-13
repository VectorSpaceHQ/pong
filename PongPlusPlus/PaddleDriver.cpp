/*
 * PaddleDriver.cpp
 *
 *  Created on: Aug 16, 2018
 *      Author: athiessen
 */

// Whether or not to use interrupts or poll mechanism
// Do not use interrupts
#undef PADDLE_CONTROL_USE_INTERRUPTS
// Use interrupts
//#define PADDLE_CONTROL_USE_INTERRUPTS

#include <Arduino.h>

// #ifdef PADDLE_CONTROL_USE_INTERRUPTS
// #include “IntHandler.h”
// #endif

#include "Configs.h"
#include "PaddleDriver.h"
#include "PaddleStatus.h"
#include "ScheduledInterval.h"
#include "Timing.h"


#define  UP_MASK     (0x66)
#define  DOWN_MASK   (0x99)

#define BUTTON_DEBOUNCE_TIME_MS     (50)


PaddleDriver::PaddleDriver(PaddleConf&      config,
                           PaddleStatus&   _status):
   ScheduledInterval(PADDLE_CHECK_INTERVAL),
   DT(config.DT),
   CLK(config.CLK),
   oldState(0),
   buttonPin(config.buttonPin),
   paddleStatus(_status),
   hwStatus(_status),
   previousButtonState(false),
   updateCount(0),
   msCount(0)
{

#ifdef PADDLE_CONTROL_USE_INTERRUPTS
   IntHandler::GetInstance()->RegisterInterrupt(DT, CLK, hwStatus.position);
#endif

}


void PaddleDriver::Update()
{
   // Check the button every time we're called
   CheckButton();

// If we are not using interrupts, then call the poll algorithm
#ifndef PADDLE_CONTROL_USE_INTERRUPTS
   CheckRotaryEncoder();
#endif

   // Periodically update the external paddle status base on our current state
   if(++updateCount >= PADDLE_UPDATE_INTERVAL)
   {
      UpdatePaddleStatus();
      updateCount = 0;
   }
}


void PaddleDriver::UpdatePaddleStatus()
{
   // If the value has changed, then increment the status accordingly
   // and reset value
   if(hwStatus.position != 0)
   {
      paddleStatus.Increment(hwStatus.position);
      hwStatus.position = 0;
   }

   if(paddleStatus.buttonPressed != hwStatus.buttonPressed)
   {
      paddleStatus.buttonStateChanged = true;
      paddleStatus.buttonPressed = hwStatus.buttonPressed;
   }

   paddleStatus.buttonTime = hwStatus.buttonTime;
}


void PaddleDriver::CheckButton()
{
   // Increment the amount of time in ms the button has been in its current state
   if(++msCount >= PADDLE_BUTTON_INCR_INTERVAL)
   {
      hwStatus.buttonTime++;
      msCount = 0;
   }

   if(hwStatus.buttonTime >= BUTTON_DEBOUNCE_TIME_MS)
   {
      bool currentStatus = (digitalRead(buttonPin) == LOW) ? true : false;

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


void PaddleDriver::CheckRotaryEncoder()
{
   uint8_t newState  = ((digitalRead(DT) << 1) | digitalRead(CLK));
   uint8_t criterion = newState ^ oldState;

   if( (criterion == 1) || (criterion == 2) )
   {
      if( UP_MASK & (1 << ( 2 * oldState + newState / 2) ) )
      {
         hwStatus.position++;
      }
      else
      {
         hwStatus.position--;       // upMask = ~downMask
      }
   }

   oldState = newState;        // Save new state
}

