/*
 * TimedInterval.h
 *
 *  Created on: Aug 25, 2018
 *      Author: athiessen
 */

#ifndef __timed_interval_h__
#define __timed_interval_h__

#include <Arduino.h>
#include <stdint.h>


class TimedInterval
{
public:
   TimedInterval(uint32_t _interal);
   virtual ~TimedInterval() {}

   void Run();

protected:
   virtual void Update() = 0;
   uint32_t interval;

private:
   uint32_t lastTime;
   uint32_t setTime;
};


inline TimedInterval::TimedInterval(uint32_t _interval):
   interval(_interval)
{
   lastTime = micros();
   setTime = lastTime + interval;
}


inline void TimedInterval::Run()
{
   // TODO: Need to handle wrap (every 70 minutes)
   if(micros() >= setTime)
   {
      Update();
      lastTime = micros();
      setTime = lastTime + interval;
   }
}


#endif   // __timed_interval_h__
