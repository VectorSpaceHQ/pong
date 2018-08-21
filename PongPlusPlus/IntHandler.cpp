/*
 * IntHandler.cpp
 *
 *  Created on: Aug 20, 2018
 *      Author: athiessen
 */

// Need to define this before including EnableInterrupt
#define EI_ARDUINO_INTERRUPTED_PIN

#include "EnableInterrupt.h"
#include "LinkedList.h"

#include "IntHandler.h"

static LinkedList<IntHandler::IntHandler_t*>   handlerList;

void IntHandler::RegisterInterrupt(uint8_t pin, IntHandler* handler)
{
   if(handler != 0)
   {
      handlerList.add(new IntHandler_t(pin, handler));
   }
}


IntHandler::~IntHandler()
{
   // TODO: To prevent memory leaks (at shutdown???)
   //    - Iterate through list
   //    - free memory for list item
   //    - delete list entry

   // For now, just clear the list
   //handlerList.clear();
}


void IntHandler::Isr()
{
   bool  found = false;
   int   cntr  = 0;
   int   size  = handlerList.size();

   while(!found && (cntr < size))
   {
      IntHandler_t* entry = handlerList.get(cntr);

      if(entry != 0)
      {
         if(entry->pinNum == arduinoInterruptedPin)
         {
            entry->handler->HandleInterrupt(arduinoInterruptedPin);
            found = true;
            break;
         }
      }
   }
}

