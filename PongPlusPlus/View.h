
#ifndef __view_h__
#define __view_h__


#include "Configs.h"
#include "LaserCtrl.h"
#include "Model.h"
#include "ScheduledInterval.h"
#include "Shape.h"


class View: public ScheduledInterval
{
public:
   View(Model::Settings&         _settings,
        Model::GameStatus&       _gameStatus,
        LaserConf&               _leftLaserConf,
        LaserConf&               _rightLaserConf,
        LaserConf&               _ballLaserConf);

   void  Update(void);
   void  Calibrate(void);

private:
   Model::Settings&        settings;
   Model::GameStatus&      gameStatus;
   LaserCtrl               leftPaddleLaser;
   LaserCtrl               rightPaddleLaser;
   LaserCtrl               ballLaser;

   void CheckGameStateChange(void);

   void SetupLaserCalibration(void);
   void SetupViewCalibration(void);
   void SetupGamePlay(void);
   void SetupGameReady(void);
   void SetupGameOver(void);

   void DisplayLaserCalibration(void);
   void DisplayViewCalibration(void);
   void DisplayHomographyCalibration(void);
   void DisplayGamePlay(void);
   void DisplayScore(void);
};


#endif    // __view_h__

