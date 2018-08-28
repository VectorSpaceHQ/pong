
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

   ButtonState             buttonState;

   void SetupLaserCalibration();

   void RunLaserCalibration();
   void RunViewCalibration();
   void CheckButtonState();
   void PrintButtonState();
};

#endif  // __engine_h__
