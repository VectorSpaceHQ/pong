/*
 * Controller.h
 *
 *  Created on: Aug 16, 2018
 *      Author: athiessen
 */

#ifndef __controller_h__
#define __controller_h__


#include "Configs.h"
#include "PaddleDriver.h"
#include "PaddleStatus.h"
#include "ScheduledInterval.h"


enum Paddles
{
   LeftPaddle  = 0,
   RightPaddle,

   // Do not modify below this line
   MaxPaddles
};


class Controller: public ScheduledInterval
{
public:
   Controller(PaddleConf& leftPaddleConf,
              PaddleConf& rightPaddleConf);

   void Update(void);

   PaddleStatus   leftPaddleStatus;
   PaddleStatus   rightPaddleStatus;

private:
   PaddleDriver     leftPaddle;
   PaddleDriver     rightPaddle;
};


#endif   //__controller_h__
