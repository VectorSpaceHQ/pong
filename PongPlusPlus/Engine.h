
#ifndef __engine_h__
#define __engine_h__

#include "Model.h"
#include "PaddleStatus.h"
#include "ScheduledInterval.h"


class Engine: public ScheduledInterval
{
   enum ButtonState
   {
      ButtonStateNone,     // Neither button is pressed
      ButtonStateLeft,     // The left button is pressed
      ButtonStateRight,    // The right button is pressed
      ButtonStateBoth,     // Both buttons are pressed
      ButtonStateReset     // We are waiting in a debouce period
   };

public:
   Engine(Model::Settings&          _settings,
          Model::GameStatus&        _gameStatus,
          PaddleStatus&             _leftPaddle,
          PaddleStatus&             _rightPaddle);

   void Update(void);

private:
   Model::Settings&        settings;
   Model::GameStatus&      gameStatus;
   PaddleStatus&           leftPaddle;
   PaddleStatus&           rightPaddle;

   // Game coords
   int16_t                 gameHeight;
   int16_t                 gameWidth;
   int8_t                  cornerCounter;

   ButtonState             buttonState;

   void SetupLaserCalibration();
   void SetupGameReady();
   void SetupGamePlay();

   void LaserCalibrationButtonChange();
   void ViewCalibrationButtonChange();
   void ReadyButtonChange();
   void GameOverButtonChange();

   void RunLaserCalibration();
   void RunViewCalibration();
   void RunHomographyCalibration();
   void ViewCalibrationComplete();
   void RunGamePlay();

   void CheckButtonState();
   void ChangeGameState(Model::GameState newState);

   void PrintButtonState();
   void PrintDisplayCoords();
   void PrintLeftPaddleCoords();
   void PrintBallCoords();
    
   void LoadSettings();
   void StoreSettings();

   void CalcHomographyMatrix(Model::LaserCalibration laserCal);
};

#endif  // __engine_h__
