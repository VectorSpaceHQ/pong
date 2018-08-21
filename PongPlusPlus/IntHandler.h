/*
 * IntHandler.h
 *
 *  Created on: Aug 20, 2018
 *      Author: athiessen
 */

#ifndef __int_handler_h__
#define __int_handler_h__

#include <stdint.h>
#include "LinkedList.h"


class IntHandler
{
public:
   struct IntHandler_t
   {
      uint8_t        pinNum;
      IntHandler*    handler;
   };

   /***
    * Constructor
    */
   IntHandler() {};
   ~IntHandler();

   void RegisterInterrupt(uint8_t pin, IntHandler* handler);

   /***
    * HandleInterrupt - This method must be overridden by the child class
    */
   void HandleInterrupt(uint8_t pin) { }

   static void Isr();

private:
   //static LinkedList<IntHandler_t*>   handlerList;

};



#endif   // __int_handler_h__
