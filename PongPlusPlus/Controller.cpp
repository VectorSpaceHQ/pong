/*
 * Controller.cpp
 *
 *  Created on: Aug 16, 2018
 *      Author: athiessen
 */

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
   leftPaddle(leftPaddleConf),
   rightPaddle(rightPaddleConf)
{
}


void Controller::Update(void)
{
   // TODO: Update paddle status based on PaddleCtrl???
}

