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


Controller::Controller(PaddleConf& leftPaddleConf,
                       PaddleConf& rightPaddleConf):
   leftPaddleStatus(),
   rightPaddleStatus(),
   leftPaddle(leftPaddleConf),
   rightPaddle(rightPaddleConf)
{
}


void Controller::Run(void)
{
   // TODO: Update paddle status based on PaddleCtrl???
}

