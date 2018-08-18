/*
 * PaddleStatus.h
 *
 *  Created on: Aug 18, 2018
 *      Author: athiessen
 */

#ifndef __paddle_status_h__
#define __paddle_status_h__

#include <stdint.h>

// For now, we'll make the paddle status a structure.
// In the future we may need something more complex
struct PaddleStatus
{

public:
   bool        buttonPressed; // Whether the button is pressed
   uint32_t    buttonTime;    // Time in ms since last button state change
   int16_t     position;      // The current position within min - max bounds

private:
   int16_t     minPosition;
   int16_t     maxPosition;

public:
   PaddleStatus():
      buttonPressed(false),
      buttonTime(0),
      position(0),            // Default to the middle
      minPosition(-32768),    // Default to absolute min limitation of the type
      maxPosition(32767)      // Default to absolute max limitation of the type
   {
   }

   void Increment(int16_t value)
   {
      uint32_t newPos = position + value;

      if(newPos > maxPosition)
      {
         newPos = maxPosition;
      }
      else if(newPos <= minPosition)
      {
         newPos = minPosition;
      }

      position = static_cast<int16_t>(newPos);
   }

   void SetLimits(int16_t _min, int16_t _max)
   {
      minPosition = _min;
      maxPosition = _max;
   }

};

#endif   // __paddle_status_h__
