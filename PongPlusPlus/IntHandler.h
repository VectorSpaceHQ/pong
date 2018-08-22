/*
 * IntHandler.h
 *
 *  Created on: Aug 20, 2018
 *      Author: athiessen
 */

#ifndef __int_handler_h__
#define __int_handler_h__

#include <stdint.h>


struct IntHandler_t
{
   uint8_t        dtPin;
   uint8_t        clkPin;
   uint8_t        oldState;
   int16_t&       position;

   IntHandler_t(int16_t& _pos):
      dtPin(0),
      clkPin(0),
      oldState(),
      position(_pos)
   {}
};

// just do it the quick and dirty way
class IntHandler
{
private:
   static IntHandler* theHandler;

public:
   static IntHandler* GetInstance();
   static void Isr();
   void RegisterInterrupt(uint8_t _dtPin, uint8_t _clkPin, int16_t& _pos);

private:
   IntHandler();
   ~IntHandler();

};


#endif   // __int_handler_h__
