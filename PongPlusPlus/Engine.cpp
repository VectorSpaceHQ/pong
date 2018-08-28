/*
 * Engine.cpp
 *
 *  Created on: Aug 16, 2018
 *      Author: athiessen
 */

#include <Arduino.h>

#include "Engine.h"
#include "Model.h"
#include "Timing.h"
#include "Sound.h"

#define  MIN_BUTTON_CHECK_ITER   (200)    // Number of iterations before re-checking the button state (debounce)


Engine::Engine(Model::Settings&           _settings,
               Model::GameStatus&         _gameStatus,
               PaddleStatus&              _leftPaddle,
               PaddleStatus&              _rightPaddle):
   ScheduledInterval(ENGINE_LOOP_INTERVAL),
   settings(_settings),
   gameStatus(_gameStatus),
   leftPaddle(_leftPaddle),
   rightPaddle(_rightPaddle),
   buttonState(ButtonStateReset)
{
   Serial.begin(115200);

   SetupLaserCalibration();
}


void Engine::Update(void)
{
   // First, update the button status
   CheckButtonState();

   switch(gameStatus.gameState)
   {
      case Model::GameStateCalibrateLasers:
         RunLaserCalibration();
         break;

      case Model::GameStateCalibrateView:
         RunViewCalibration();
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


void Engine::SetupLaserCalibration()
{
   // Center for the laser
   leftPaddle.position = 0;
   rightPaddle.position = 0;

   leftPaddle.SetLimits(-500, 500);
   rightPaddle.SetLimits(-500, 500);
   gameStatus.gameState = Model::GameStateCalibrateLasers;
}


void Engine::LaserCalibrationButtonChange()
{
   if((leftPaddle.buttonStateChanged) || (rightPaddle.buttonStateChanged))
   {
      switch(buttonState)
      {
         case ButtonStateNone:
            leftPaddle.position = settings.middleLaserCal.xOffset;
            rightPaddle.position = settings.middleLaserCal.yOffset;
            break;

         case ButtonStateLeft:
            leftPaddle.position = settings.leftLaserCal.xOffset;
            rightPaddle.position = settings.leftLaserCal.yOffset;
            break;

         case ButtonStateRight:
            leftPaddle.position = settings.rightLaserCal.xOffset;
            rightPaddle.position = settings.rightLaserCal.yOffset;
            break;
      }

      leftPaddle.buttonStateChanged = false;
      rightPaddle.buttonStateChanged = false;
   }
}


void Engine::RunLaserCalibration()
{
   leftPaddle.SetLimits(-500, 500);
   rightPaddle.SetLimits(-500, 500);

   LaserCalibrationButtonChange();

   switch(buttonState)
   {
      // Update X/Y position of the display if no buttons pressed
      case ButtonStateNone:
         settings.middleLaserCal.xOffset = leftPaddle.position;    // Left paddle controls X-Axis
         settings.middleLaserCal.yOffset = rightPaddle.position;   // Right paddle controls Y-Axis
         break;

      // Update size of the display if the left button is pressed
      case ButtonStateLeft:
         settings.leftLaserCal.xOffset = leftPaddle.position;    // Left paddle controls X-Axis
         settings.leftLaserCal.yOffset = rightPaddle.position;   // Right paddle controls Y-Axis
         break;

      // Update skew of the display if the right button is pressed
      case ButtonStateRight:
         settings.rightLaserCal.xOffset = leftPaddle.position;    // Left paddle controls X-Axis
         settings.rightLaserCal.yOffset = rightPaddle.position;   // Right paddle controls Y-Axis
         break;

      // If both buttons are pressed, end configuration
      case ButtonStateBoth:
         PlayPointSound();
         gameStatus.gameState = Model::GameStateCalibrateView;
         break;
   }
}


void Engine::RunViewCalibration()
{
   switch(buttonState)
   {
      // Update X/Y position of the display if no buttons pressed
      case ButtonStateNone:
         settings.display.xMin = leftPaddle.position;    // Left paddle controls X-Axis
         settings.display.yMin = rightPaddle.position;   // Right paddle controls Y-Axis
         break;

      // Update size of the display if the left button is pressed
      case ButtonStateLeft:
         settings.display.xMax = leftPaddle.position;    // Left paddle controls X-Axis
         settings.display.yMax = rightPaddle.position;   // Right paddle controls Y-Axis
         break;

      // Update skew of the display if the right button is pressed
      case ButtonStateRight:
         settings.display.hSkew = leftPaddle.position;   // Left paddle controls horizontal skew
         settings.display.vSkew = rightPaddle.position;  // Right paddle controls vertical skew
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
      buttonState = ButtonStateNone;
   }

   //PrintButtonState();
}


void Engine::PrintButtonState()
{
   Serial.print("Left( ");
   Serial.print(leftPaddle.position);
   Serial.print(", ");
   Serial.print(leftPaddle.buttonPressed);
   Serial.print(", ");
   Serial.print(leftPaddle.buttonTime);
   Serial.print(" );  Right( ");
   Serial.print(rightPaddle.position);
   Serial.print(", ");
   Serial.print(rightPaddle.buttonPressed);
   Serial.print(", ");
   Serial.print(rightPaddle.buttonTime);
   Serial.print(" ) -> ");
   Serial.println(buttonState);
}
