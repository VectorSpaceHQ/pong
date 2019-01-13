/*
 * ScheduledInterval.h
 *
 *  Created on: Aug 18, 2018
 *      Author: athiessen
 */

#ifndef __scheduled_interval_h__
#define __scheduled_interval_h__

#include <stdint.h>

/******************************************************************************
 * The ScheduledInterval base class calls the Update method on its derived
 * class every time the interval count gets to the configured value
 ******************************************************************************/
class ScheduledInterval
{
public:
   ScheduledInterval(uint32_t _interal);
   virtual ~ScheduledInterval() {}

   void Run();

protected:
   virtual void Update() = 0;

private:
   uint32_t interval;
   uint32_t scheduleCount;
};


inline ScheduledInterval::ScheduledInterval(uint32_t _interval):
   interval(_interval),
   scheduleCount(0)
{
}


inline void ScheduledInterval::Run()
{
   if(++scheduleCount >= interval)
   {
      Update();
      scheduleCount = 0;
   }
}


#endif   // __scheduled_interval_h__
