/*
 * IntHandler.cpp
 *
 *  Created on: Aug 20, 2018
 *      Author: athiessen
 */

// Need to define this before including EnableInterrupt
#define EI_ARDUINO_INTERRUPTED_PIN

#include <Arduino.h>
#include <EnableInterrupt.h>
#include <LinkedList.h>

#include "IntHandler.h"


#define  UP_MASK     (0x66)
#define  DOWN_MASK   (0x99)

IntHandler* IntHandler::theHandler = nullptr;
static LinkedList<IntHandler_t*>   handlerList = LinkedList<IntHandler_t*>();

IntHandler::IntHandler() { }


IntHandler::~IntHandler()
{
   // TODO: To prevent memory leaks (at shutdown???)
   //    - Iterate through list
   //    - free memory for list item
   //    - delete list entry
   handlerList.clear();
}


IntHandler* IntHandler::GetInstance()
{
   if(theHandler == nullptr)
   {
      theHandler = new IntHandler();
   }

   return theHandler;
}


void IntHandler::RegisterInterrupt(uint8_t _dtPin, uint8_t _clkPin, int16_t& _pos)
{
   IntHandler_t* newEntry = new IntHandler_t(_pos);
   newEntry->dtPin = _dtPin;
   newEntry->clkPin = _clkPin;
   newEntry->position = _pos;
   newEntry->oldState = 0;
   handlerList.add(newEntry);
   enableInterrupt(_dtPin, Isr, CHANGE);
   enableInterrupt(_clkPin, Isr, CHANGE);

   Serial.print(_dtPin);
   Serial.print(", ");
   Serial.println(_clkPin);
}


void IntHandler::Isr()
{
   bool  found = false;
   int   cntr  = 0;
   int   size  = handlerList.size();
   IntHandler_t* entry;

   while(!found && (cntr < size))
   {
      entry = handlerList.get(cntr);

      if(entry != nullptr)
      {
         if((entry->dtPin == arduinoInterruptedPin) || (entry->clkPin == arduinoInterruptedPin))
         {
            uint8_t newState  = ((digitalRead(entry->dtPin) << 1) | digitalRead(entry->clkPin));
            uint8_t criterion = newState ^ entry->oldState;

            if( (criterion == 1) || (criterion == 2) )
            {
               if( UP_MASK & (1 << ( 2 * entry->oldState + newState / 2) ) )
               {
                  entry->position++;
               }
               else
               {
                  entry->position--;       // upMask = ~downMask
               }
            }

            entry->oldState = newState;        // Save new state

            found = true;
            break;
         }

         ++cntr;
      }
   }
}
