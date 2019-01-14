/*
 * AtariPaddleDriver.h
 *
 *  Created on: Aug 16, 2018
 *      Author: athiessen
 */

#ifndef __atari_paddle_driver_h__
#define __atari_paddle_driver_h__

#include <stdint.h>

#include "Configs.h"
#include "GamePadStatus.h"
// #include "IntHandler.h"
#include "ScheduledInterval.h"


class AtariPaddleDriver: public ScheduledInterval
{
public:
   AtariPaddleDriver(PaddleConf&     config,
                     GamePadStatus&   _status);

private:
   uint8_t              DT;                  // Rotary Encoder DT Pin
   uint8_t              CLK;                 // Rotary Encoder CLK Pin
   uint8_t              buttonPin;           // The pin used for the button
   uint8_t              oldState;            // Previous state of the rotary encoder
   GamePadStatus&       paddleStatus;        // A reference to the PaddleStatus object to periodically update for the game engine
   GamePadStatus        hwStatus;            // A local copy of the current status of the paddle HW.
   bool                 previousButtonState; // The previous state of the button: used to detect if the button has changed
   uint32_t             updateCount;         // Counter for how often we update the external paddle status
   uint32_t             msCount;             // Tracks how long in ms the button has been in its current state

   void Update();
   void UpdatePaddleStatus();
   void CheckButton();
   void CheckRotaryEncoder();
};


#endif   // __atari_paddle_driver_h__
