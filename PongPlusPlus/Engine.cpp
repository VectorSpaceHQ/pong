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
#include <EEPROM.h>


#define  MIN_BUTTON_CHECK_ITER   (200)    // Number of iterations before re-checking the button state (debounce)
#define  MAX_SCORE               (9)


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
   Serial.println("Engine Up");
}


void Engine::Update(void)
{
   // First, update the button status
   CheckButtonState();

   switch(gameStatus.gameState)
   {
      case Model::GameStateInit:
         SetupLaserCalibration();
         break;

      case Model::GameStateCalibrateLasers:
         RunLaserCalibration();
         break;

      case Model::GameStateCalibrateView:
         RunViewCalibration();
         break;

      case Model::GameStateReady:
         ReadyButtonChange();
         break;

      case Model::GameStatePlay:
         RunGamePlay();
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

   ChangeGameState(Model::GameStateCalibrateLasers);
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
            ChangeGameState(Model::GameStateCalibrateView);
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
            ChangeGameState(Model::GameStateReady);
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
            gameStatus.viewSettingsChanged = true;
         }

         if(settings.display.yMin != leftPaddle.position)
         {
            settings.display.yMin = leftPaddle.position;   // Right paddle controls Y-Axis
            gameStatus.viewSettingsChanged = true;
         }
         break;

      // Update size of the display if the left button is pressed
      case ButtonStateLeft:
         if(settings.display.xMax != rightPaddle.position)
         {
            settings.display.xMax = rightPaddle.position;    // Left paddle controls X-Axis
            gameStatus.viewSettingsChanged = true;
         }

         if(settings.display.yMax != leftPaddle.position)
         {
            settings.display.yMax = leftPaddle.position;   // Right paddle controls Y-Axis
            gameStatus.viewSettingsChanged = true;
         }
         break;

      // Update skew of the display if the right button is pressed
      case ButtonStateRight:
         if(settings.display.hSkew != rightPaddle.position)
         {
            settings.display.hSkew = rightPaddle.position;   // Left paddle controls horizontal skew
            gameStatus.viewSettingsChanged = true;
         }

         if(settings.display.vSkew != leftPaddle.position)
         {
            settings.display.vSkew = leftPaddle.position;  // Right paddle controls vertical skew
            gameStatus.viewSettingsChanged = true;
         }
         break;
   }
}


void Engine::ReadyButtonChange()
{
   // Users can move paddles in the ready state
   gameStatus.leftPaddleShape.position.x  =     (settings.display.xMin + settings.display.xMax) / 4;
   gameStatus.rightPaddleShape.position.x = 3 * (settings.display.xMin + settings.display.xMax) / 4;

   // Whoever scored last gets to serve
   if((leftPaddle.buttonStateChanged) || (rightPaddle.buttonStateChanged))
   {
      switch(buttonState)
      {
         case ButtonStateLeft:
            if(gameStatus.whoseServe == Model::LeftPlayerServes)
            {
               ChangeGameState(Model::GameStatePlay);
            }
            else if(gameStatus.whoseServe == Model::EitherPlayerServes)
            {
               gameStatus.whoseServe = Model::LeftPlayerServes;

               ChangeGameState(Model::GameStatePlay);
            }
            break;

         case ButtonStateRight:
            if(gameStatus.whoseServe == Model::RightPlayerServes)
            {
               ChangeGameState(Model::GameStatePlay);
            }
            else if(gameStatus.whoseServe == Model::EitherPlayerServes)
            {
               gameStatus.whoseServe = Model::RightPlayerServes;

               ChangeGameState(Model::GameStatePlay);
            }
            break;

         default:
            // Do nothing, just wait for the correct player to serve
            break;
      }

      leftPaddle.buttonStateChanged = false;
      rightPaddle.buttonStateChanged = false;
   }
}


void Engine::SetupGameReady()
{
   // Paddles are at a fixed horizontal location
   gameStatus.leftPaddleShape.position.x  =     (settings.display.xMin + settings.display.xMax) / 4;
   gameStatus.rightPaddleShape.position.x = 3 * (settings.display.xMin + settings.display.xMax) / 4;
   gameStatus.leftPaddleShape.position.y  = 0;
   gameStatus.rightPaddleShape.position.y = 0;
}


void Engine::SetupGamePlay()
{
   randomSeed(micros());

   // Paddles are at a fixed horizontal location
   gameStatus.leftPaddleShape.position.x  =     (settings.display.xMin + settings.display.xMax) / 4;
   gameStatus.rightPaddleShape.position.x = 3 * (settings.display.xMin + settings.display.xMax) / 4;

   // Randomly select top third or bottom third
   if(random(1) == 1)
   {
      // Select top 1/3
      gameStatus.ballShape.position.y = 2 * (settings.display.yMax + settings.display.yMin) / 3;
   }
   else
   {
      // Select bottom 1/3
      gameStatus.ballShape.position.y = (settings.display.yMax + settings.display.yMin) / 3;
   }

   // Start the ball in the horizontal center
   gameStatus.ballShape.position.x = 0;

   // TODO: Randomize the y-component of the vector
   // Select ball x vector
   if(gameStatus.whoseServe == Model::LeftPlayerServes)
   {
      // If the left player is serving, set the vector to a positive (right) direction
      gameStatus.ballShape.vector.x = 2;
   }
   else
   {
      // If the left player is serving, set the vector to a negative (left) direction
      gameStatus.ballShape.vector.x = -2;
   }

   // Randomize up/down
   if(random(1) == 1)
   {
      // Down, y is positive
      gameStatus.ballShape.vector.y = random(2, 5);   // Slope is random of 2/2, 3/2, 4/2, or 5/2
   }
   else
   {
      // Up, y is negative
      gameStatus.ballShape.vector.y =  -random(2, 5);
   }
}


void Engine::RunGamePlay()
{
   Vertex      foundVertex;

   // Move the paddles
   // TODO: We probably need to convert the position into an actual location
   // TODO: We should probably implement some form of scaling to prevent over-movement
   gameStatus.leftPaddleShape.Move(CoordsWorld, 0, (leftPaddle.position - gameStatus.leftPaddleShape.position.y));
   gameStatus.rightPaddleShape.Move(CoordsWorld, 0, (rightPaddle.position - gameStatus.rightPaddleShape.position.y));

   // Move the ball along it's trajectory
   gameStatus.ballShape.Move(CoordsWorld, gameStatus.ballShape.vector.x, gameStatus.ballShape.vector.y);

   // Check collision of the ball with the top or bottom
   if( (gameStatus.ballShape.CheckTop(settings.display.yMax, foundVertex)    ) ||
       (gameStatus.ballShape.CheckBottom(settings.display.yMin, foundVertex) )    )
   {
      // Ball hit the top or bottom, so invert the y-component of the slope
      gameStatus.ballShape.vector.y *= -1;

      PlayWallSound();
   }

   // If the ball is traveling left, then check it for collision with the left paddle
   if(gameStatus.ballShape.vector.x < 0)
   {
      // If the ball's left-most vertex is between the highest and lowest paddle vertices...
      if((gameStatus.ballShape.leftMostVertex.y <= gameStatus.leftPaddleShape.highestVertex.y) &&
         (gameStatus.ballShape.leftMostVertex.y >= gameStatus.leftPaddleShape.lowestVertex.y )    )
      {
         // And the it's beyond the paddle edge
         if(gameStatus.ballShape.CheckLeft(gameStatus.leftPaddleShape.rightMostVertex.x, foundVertex))
         {
            // Ball hit the left paddle so invert the x-component of the slope
            gameStatus.ballShape.vector.x *= -1;

            PlayPaddleSound();

            // TODO: take in velocity of the paddle to adjust slope of the ball
         }
      }

      // Check to see if the ball has reached the left edge
      if(gameStatus.ballShape.CheckLeft(settings.display.xMin, foundVertex))
      {
         gameStatus.rightPaddleScore++;
         gameStatus.whoseServe = Model::RightPlayerServes;
         PlayPointSound();
         ChangeGameState(Model::GameStateGameOver);
      }
   }

   // If the ball is traveling right, then check it for collision with the right paddle
   if(gameStatus.ballShape.vector.x > 0)
   {
      // If the ball's left-most vertex is between the highest and lowest paddle vertices...
      if((gameStatus.ballShape.rightMostVertex.y <= gameStatus.rightPaddleShape.highestVertex.y) &&
         (gameStatus.ballShape.rightMostVertex.y >= gameStatus.rightPaddleShape.lowestVertex.y )    )
      {
         // And the it's beyond the paddle edge
         if(gameStatus.ballShape.CheckRight(gameStatus.rightPaddleShape.leftMostVertex.x, foundVertex))
         {
            // Ball hit the left paddle so invert the x-component of the slope
            gameStatus.ballShape.vector.x *= -1;

            PlayPaddleSound();

            // TODO: take in velocity of the paddle to adjust slope of the ball
         }
      }

      // Check to see if the ball has reached the right edge
      if(gameStatus.ballShape.CheckRight(settings.display.xMax, foundVertex))
      {
         gameStatus.leftPaddleScore++;
         gameStatus.whoseServe = Model::LeftPlayerServes;
         PlayPointSound();
         ChangeGameState(Model::GameStateGameOver);
      }
   }
}


void Engine::GameOverButtonChange()
{
   // Either button will take us back to the ready state
   if((leftPaddle.buttonStateChanged) || (rightPaddle.buttonStateChanged))
   {
      switch(buttonState)
      {
         case ButtonStateLeft:
         case ButtonStateRight:
            ChangeGameState(Model::GameStateReady);

            if( (gameStatus.leftPaddleScore >= MAX_SCORE) ||
                (gameStatus.rightPaddleScore >= MAX_SCORE)   )
            {
               gameStatus.leftPaddleScore = 0;
               gameStatus.rightPaddleScore = 0;
               gameStatus.whoseServe = Model::EitherPlayerServes;
            }
            break;
      }

      leftPaddle.buttonStateChanged = false;
      rightPaddle.buttonStateChanged = false;
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


void Engine::ChangeGameState(Model::GameState newState)
{
   // Notify the View of the new game state
   gameStatus.gameState = newState;
   gameStatus.gameStateChanged = true;

   switch(newState)
   {
      case Model::GameStateCalibrateLasers:
         PlayPaddleSound();
         Serial.println("New Game State: Calibrate Lasers");
         break;

      case Model::GameStateCalibrateView:
         PlayPaddleSound();
         Serial.println("New Game State: Calibrate View");
         break;

      case Model::GameStateReady:
         SetupGameReady();
         Serial.println("New Game State: Ready");
         break;

      case Model::GameStatePlay:
         SetupGamePlay();
         Serial.println("New Game State: Play");
         break;

      case Model::GameStateGameOver:
         Serial.println("New Game State: Game Over");

         Serial.print("Score: ");
         Serial.print(gameStatus.leftPaddleScore);
         Serial.print(" - ");
         Serial.println(gameStatus.rightPaddleScore);
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

void Engine::LoadSettings(){
  settings.display.xMin = EEPROM.read(0);
  delay(50);
  settings.display.yMin = EEPROM.read(4);
  delay(50);
  settings.display.xMax = EEPROM.read(8);
  delay(50);
  settings.display.yMax = EEPROM.read(12);
  delay(50);
  settings.display.hSkew = EEPROM.read(16);
  delay(50);
  settings.display.vSkew = EEPROM.read(20);
  delay(50);
}

void Engine::SaveSettings(){
  EEPROM.write(0, settings.display.xMin);
  delay(50); // An EEPROM write takes 3.3 ms to complete.
  EEPROM.write(4, settings.display.yMin);
  delay(50); 
  EEPROM.write(8, settings.display.xMax);
  delay(50); 
  EEPROM.write(12, settings.display.yMax);
  delay(50); 
  EEPROM.write(16, settings.display.hSkew);
  delay(50); 
  EEPROM.write(20, settings.display.vSkew);
  delay(50); 
}
