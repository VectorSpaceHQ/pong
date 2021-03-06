
#ifndef __engine_h__
#define __engine_h__

#include "GamePadStatus.h"
#include "Model.h"
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
          GamePadStatus&             _leftPaddle,
          GamePadStatus&             _rightPaddle);

   void Update(void);

private:
   Model::Settings&        settings;
   Model::GameStatus&      gameStatus;
   GamePadStatus&           leftPaddle;
   GamePadStatus&           rightPaddle;

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
   void RunGamePlay();

   void CheckButtonState();
   void ChangeGameState(Model::GameState newState);

   void PrintButtonState();
   void PrintDisplayCoords();
   void PrintLeftPaddleCoords();
   void PrintRightPaddleCoords();
   void PrintBallCoords();
    
   void LoadSettings();
   void StoreSettings();
};

#endif  // __engine_h__
