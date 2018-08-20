/*
 * Engine.cpp
 *
 *  Created on: Aug 16, 2018
 *      Author: athiessen
 */

#include "Engine.h"
#include "Model.h"
#include "Timing.h"

#define  MIN_BUTTON_CHECK_ITER   (200)    // Number of iterations before re-checking the button state (debounce)


Engine::Engine(Model::DisplaySettings&    _display,
               Model::GameStatus&         _gameStatus,
               PaddleStatus&              _leftPaddle,
               PaddleStatus&              _rightPaddle):
   ScheduledInterval(ENGINE_LOOP_INTERVAL),
   display(_display),
   gameStatus(_gameStatus),
   leftPaddle(_leftPaddle),
   rightPaddle(_rightPaddle),
   buttonState(ButtonStateReset)
{
}


void Engine::Update(void)
{
   // First, update the button status
   CheckButtonState();

   switch(gameStatus.gameState)
   {
      case Model::GameStateCalibrate:
         RunCalibration();
         break;

      case Model::GameStateReady:
         break;

      case Model::GameStatePlay:
         break;

      case Model::GameStateGameOver:
      default:
         break;
   }
}


void Engine::RunCalibration()
{
   switch(buttonState)
   {
      // Update X/Y position of the display if no buttons pressed
      case ButtonStateNone:
         display.xMin = leftPaddle.position;    // Left paddle controls X-Axis
         display.yMin = rightPaddle.position;   // Right paddle controls Y-Axis
         break;

      // Update size of the display if the left button is pressed
      case ButtonStateLeft:
         display.xMax = leftPaddle.position;    // Left paddle controls X-Axis
         display.yMax = rightPaddle.position;   // Right paddle controls Y-Axis
         break;

      // Update skew of the display if the right button is pressed
      case ButtonStateRight:
         display.hSkew = leftPaddle.position;   // Left paddle controls horizontal skew
         display.vSkew = rightPaddle.position;  // Right paddle controls vertical skew
         break;

      // If both buttons are pressed, end configuration
      case ButtonStateBoth:
         gameStatus.gameState = Model::GameStateReady;
         break;
   }
}


void Engine::CheckButtonState()
{
   if(leftPaddle.buttonPressed && rightPaddle.buttonPressed)
   {
      buttonState = ButtonStateBoth;
   }
   else if(leftPaddle.buttonPressed)
   {
      buttonState = ButtonStateLeft;
   }
   else if(rightPaddle.buttonPressed)
   {
      buttonState = ButtonStateRight;
   }
   else
   {
      buttonState == ButtonStateNone;
   }
}
