/*
 * PaddleCtrl.h
 *
 *  Created on: Aug 16, 2018
 *      Author: athiessen
 */

#ifndef __paddle_ctrl_h__
#define __paddle_ctrl_h__

#include <stdint.h>

#include "Configs.h"

class PaddleCtrl
{
public:
   PaddleCtrl(PaddleConf& config);

private:
   uint8_t  pinA;       // Rotary Pin A
   uint8_t  pinB;       // Rotary Pin B
   uint8_t  buttonPin;

};



#endif   // __paddle_ctrl_h__
