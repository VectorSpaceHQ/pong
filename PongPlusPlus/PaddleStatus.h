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
   bool        buttonPressed;
   uint32_t    position;

   PaddleStatus():
      buttonPressed(false),
      position(0)
   {
   }

};

#endif   // __paddle_status_h__
