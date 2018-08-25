
#ifndef __view_h__
#define __view_h__


#include "Configs.h"
#include "LaserCtrl.h"
#include "Model.h"
#include "NumberShapes.h"
#include "ScheduledInterval.h"
#include "Shape.h"


class View: public ScheduledInterval
{
public:
   View(Model::DisplaySettings&  _display,
        Model::GameStatus&       _gameStatus,
        LaserConf&               _leftLaserConf,
        LaserConf&               _rightLaserConf,
        LaserConf&               _ballLaserConf);

   void  Update(void);
   void  Calibrate(void);

private:
   Model::DisplaySettings& display;
   Model::GameStatus&      gameStatus;
   NumberShapes            numberShapes;
   LaserCtrl               leftPaddleLaser;
   LaserCtrl               rightPaddleLaser;
   LaserCtrl               ballLaser;

   void DisplayCalibration(void);
   void DisplayGamePlay(void);
   void DisplayScore(void);
};


#endif    // __view_h__

