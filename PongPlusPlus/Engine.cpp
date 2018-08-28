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

   UpdateGameState(Model::GameStateCalibrateLasers);
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

         // If both buttons are pressed, end configuration
         case ButtonStateBoth:
            UpdateGameState(Model::GameStateCalibrateView);
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
   }
}


void Engine::ViewCalibrationButtonChange()
{
   if((leftPaddle.buttonStateChanged) || (rightPaddle.buttonStateChanged))
   {
      switch(buttonState)
      {
         case ButtonStateNone:
            leftPaddle.SetLimits(-500, settings.display.yMax);
            rightPaddle.SetLimits(-500, settings.display.xMax);
            leftPaddle.position = settings.display.yMin;
            rightPaddle.position = settings.display.xMin;
            break;

         case ButtonStateLeft:
            leftPaddle.SetLimits(settings.display.yMin, 500);
            rightPaddle.SetLimits(settings.display.xMin, 500);
            leftPaddle.position = settings.display.yMax;
            rightPaddle.position = settings.display.xMax;
            break;

         case ButtonStateRight:
            leftPaddle.SetLimits(settings.display.yMin, settings.display.yMax);
            rightPaddle.SetLimits(settings.display.xMin, settings.display.xMax);
            leftPaddle.position = settings.display.vSkew;
            rightPaddle.position = settings.display.hSkew;
            break;

         // If both buttons are pressed, end configuration
         case ButtonStateBoth:
            UpdateGameState(Model::GameStateReady);
            break;
      }

      leftPaddle.buttonStateChanged = false;
      rightPaddle.buttonStateChanged = false;
   }
}


void Engine::RunViewCalibration()
{
   ViewCalibrationButtonChange();

   switch(buttonState)
   {
      // Update X/Y position of the display if no buttons pressed
      case ButtonStateNone:
         if(settings.display.xMin != rightPaddle.position)
         {
            // TODO: ensure these can't cross
            settings.display.xMin = rightPaddle.position;    // Left paddle controls X-Axis
            gameStatus.gameStateChanged = true;
         }

         if(settings.display.yMin != leftPaddle.position)
         {
            settings.display.yMin = leftPaddle.position;   // Right paddle controls Y-Axis
            gameStatus.gameStateChanged = true;
         }
         break;

      // Update size of the display if the left button is pressed
      case ButtonStateLeft:
         if(settings.display.xMax != rightPaddle.position)
         {
            settings.display.xMax = rightPaddle.position;    // Left paddle controls X-Axis
            gameStatus.gameStateChanged = true;
         }

         if(settings.display.yMax != leftPaddle.position)
         {
            settings.display.yMax = leftPaddle.position;   // Right paddle controls Y-Axis
            gameStatus.gameStateChanged = true;
         }
         break;

      // Update skew of the display if the right button is pressed
      case ButtonStateRight:
         if(settings.display.hSkew != rightPaddle.position)
         {
            settings.display.hSkew = rightPaddle.position;   // Left paddle controls horizontal skew
            gameStatus.gameStateChanged = true;
         }

         if(settings.display.vSkew != leftPaddle.position)
         {
            settings.display.vSkew = leftPaddle.position;  // Right paddle controls vertical skew
            gameStatus.gameStateChanged = true;
         }
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


void Engine::UpdateGameState(Model::GameState newState)
{
   gameStatus.gameState = newState;
   gameStatus.gameStateChanged = true;
   PlayPaddleSound();

   switch(newState)
   {
      case Model::GameStateCalibrateLasers:
         Serial.println("New Game State: Calibrate Lasers");
         break;

      case Model::GameStateCalibrateView:
         Serial.println("New Game State: Calibrate View");
         break;

      case Model::GameStateReady:
         Serial.println("New Game State: Ready");
         break;

      case Model::GameStatePlay:
         Serial.println("New Game State: Play");
         break;

      case Model::GameStateGameOver:
         Serial.println("New Game State: Game Over");
         break;
   }
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
