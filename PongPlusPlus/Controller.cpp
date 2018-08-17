/*
 * Controller.cpp
 *
 *  Created on: Aug 16, 2018
 *      Author: athiessen
 */

#include "Configs.h"
#include "Controller.h"


Controller::Controller(PaddleConf& leftPaddleConf,
                       PaddleConf& rightPaddleConf):
   leftPaddle(leftPaddleConf),
   rightPaddle(rightPaddleConf)
{
}


void Controller::Run(void)
{
   // Hmm.... is there anything to do here?? Perhaps debounce the button?
}

