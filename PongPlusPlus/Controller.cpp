/*
 * Controller.cpp
 *
 *  Created on: Aug 16, 2018
 *      Author: athiessen
 */

#include <Arduino.h>

#include "Configs.h"
#include "Controller.h"
#include "PaddleStatus.h"
#include "PaddleCtrl.h"
#include "ScheduledInterval.h"
#include "Timing.h"


Controller::Controller(PaddleConf& leftPaddleConf,
                       PaddleConf& rightPaddleConf):
   ScheduledInterval(CONTROLLER_LOOP_INTERVAL),
   leftPaddleStatus(),
   rightPaddleStatus(),
   leftPaddle(leftPaddleConf,
              leftPaddleStatus),
   rightPaddle(rightPaddleConf,
               rightPaddleStatus)
{
   Serial.println("Controller Up");
}


void Controller::Update(void)
{
   leftPaddle.Run();
   rightPaddle.Run();
}

