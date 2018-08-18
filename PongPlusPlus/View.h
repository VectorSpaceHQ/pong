
#ifndef __view_h__
#define __view_h__


#include "Configs.h"
#include "LaserCtrl.h"
#include "Shapes.h"


class View
{
public:
   View(GameStatus&  _gameStatus,
        Circle&      _ball,
        Rectangle&   _leftPaddle,
        Rectangle&   _rightPaddle,
        LaserConf&   _leftLaserConf,
        LaserConf&   _rightLaserConf,
        LaserConf&   _ballLaserConf);

   void  Run(void);
   void  Calibrate(void);

private:
   GameStatus& gameStatus;
   Circle&     ball;
   Rectangle&  leftPaddle;
   Rectangle&  rightPaddle;

   LaserCtrl   leftPaddleLaser;
   LaserCtrl   rightPaddleLaser;
   LaserCtrl   ballLaser;

   void DisplayCalibration(void);
   void DisplayGamePlay(void);
   void DisplayScore(void);
};


#endif    // __view_h__

