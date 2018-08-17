/*
 * PaddleCtrl.cpp
 *
 *  Created on: Aug 16, 2018
 *      Author: athiessen
 */

#include "Configs.h"
#include "PaddleCtrl.h"


PaddleCtrl::PaddleCtrl(PaddleConf& config):
   upPin(config.upPin),
   downPin(config.upPin),
   buttonPin(config.buttonPin)
{
}

