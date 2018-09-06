/*
 * Engine.cpp
 *
 *  Created on: Aug 16, 2018
 *      Author: athiessen
 */

#include <Arduino.h>
#include <EEPROM.h>
#include <stdio.h>

#include "Engine.h"
#include "Model.h"
#include "Sound.h"
#include "Timing.h"

// TODO: What should be the scale of the paddles?
#define PADDLE_SCALE_PERCENT        (10)            // Percent of the height of the paddle
#define BALL_SCALE_PERCENT          (2)            // Percent of the height of the ball

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
   buttonState(ButtonStateReset),
   gameHeight(0),
   gameWidth(0)
{
   Serial.println("Engine Up");
   randomSeed(micros());
}


void Engine::Update(void)
{
  PrintLeftPaddleCoords();
  PrintRightPaddleCoords();
  
   // First, update the button status
   CheckButtonState();

   switch(gameStatus.gameState)
   {
      case Model::GameStateInit:
         LoadSettings();
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
         GameOverButtonChange();
         break;
   }
}


void Engine::LoadSettings()
{
   Model::Settings   localSettings;
   uint8_t*          readPtr        = reinterpret_cast<uint8_t*>(&localSettings);
   int               address        = 0;
   int               checksum       = 0;
   int               imgChecksum;

   Serial.println("LOADING SETTINGS");

   settings.leftLaserCal.xOffset = -57;
   settings.leftLaserCal.yOffset = -157;
   settings.middleLaserCal.xOffset = -131;
   settings.middleLaserCal.yOffset = -194;
   settings.rightLaserCal.xOffset = -15;
   settings.rightLaserCal.yOffset = -196;

   settings.display.xMin = -122;
   settings.display.xMax = 93;
   settings.display.yMin = -75;
   settings.display.yMax = 104;

   // // Read the bytes out of EEPROM and calculate the checksum
   // for(address = 0; address < sizeof(localSettings); address++)
   // {
   //    readPtr[address] = EEPROM.read(address);
   //    checksum ^= readPtr[address] & 0xFF;
   // }

   
   // Serial.println(checksum);
   
   // if(checksum == readPtr[0])
   // {
   //    Serial.println("Loading Settings");
   //    memcpy(&settings, &localSettings, sizeof(localSettings));
   //    Serial.println(localSettings.leftLaserCal.xOffset);
   // }
   
}


// void Engine::StoreSettings()
// {
//    Model::Settings   localSettings;
//    uint8_t*          readPtr        = reinterpret_cast<uint8_t*>(&localSettings);
//    int               address        = 0;
//    int               checksum       = 0;
//    int               imgChecksum;


//    // Read the bytes out of EEPROM and calculate the checksum
//    for(address = 0; address < sizeof(localSettings); address++)
//    {
//       readPtr[address] = EEPROM.read(address);
//       checksum ^= readPtr[address] & 0xFF;
//    }

//    if(checksum == readPtr[0])
//      {
//        for(address = 0; address < sizeof(localSettings); address++)
//          {
//            EEPROM.write(address, localSettings[address]);
//          }
//      }
// }


void Engine::SetupLaserCalibration()
{
   // use laser position from EEPROM
   leftPaddle.position = settings.middleLaserCal.xOffset;
   rightPaddle.position = settings.middleLaserCal.yOffset;

   leftPaddle.SetLimits(-500, 500);
   rightPaddle.SetLimits(-500, 500);

   ChangeGameState(Model::GameStateCalibrateLasers);
}


void Engine::LaserCalibrationButtonChange()
{
   if((leftPaddle.buttonStateChanged) || (rightPaddle.buttonStateChanged))
   {
      PrintButtonState();
      
      delay(300); // debounce

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
         Serial.print("Middle: (");
         Serial.print(settings.middleLaserCal.xOffset);
         Serial.print(", ");
         Serial.print(settings.middleLaserCal.yOffset);
         Serial.println(" )");
         break;

      // Update size of the display if the left button is pressed
      case ButtonStateLeft:
         settings.leftLaserCal.xOffset = leftPaddle.position;    // Left paddle controls X-Axis
         settings.leftLaserCal.yOffset = rightPaddle.position;   // Right paddle controls Y-Axis
         Serial.print("Left: (");
         Serial.print(settings.leftLaserCal.xOffset);
         Serial.print(", ");
         Serial.print(settings.leftLaserCal.yOffset);
         Serial.println(" )");
         break;

      // Update skew of the display if the right button is pressed
      case ButtonStateRight:
         settings.rightLaserCal.xOffset = leftPaddle.position;    // Left paddle controls X-Axis
         settings.rightLaserCal.yOffset = rightPaddle.position;   // Right paddle controls Y-Axis
         Serial.print("Right: (");
         Serial.print(settings.rightLaserCal.xOffset);
         Serial.print(", ");
         Serial.print(settings.rightLaserCal.yOffset);
         Serial.println(" )");
         break;
   }
}


void Engine::ViewCalibrationButtonChange()
{
   if((leftPaddle.buttonStateChanged) || (rightPaddle.buttonStateChanged))
   {
      PrintButtonState();

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
            ViewCalibrationComplete();
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


void Engine::ViewCalibrationComplete()
{
   // Set the game dimensions
   gameHeight = settings.display.yMax - settings.display.yMin;
   gameWidth = settings.display.xMax - settings.display.xMin;
}


void Engine::ReadyButtonChange()
{
   // Users can move paddles in the ready state
   gameStatus.leftPaddleShape.Move(CoordsWorld, 0, (leftPaddle.position - gameStatus.leftPaddleShape.position.y));
   gameStatus.rightPaddleShape.Move(CoordsWorld, 0, (rightPaddle.position - gameStatus.rightPaddleShape.position.y));

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
   // We'll setup the shapes for both Game Ready and Game Play states here.
  uint32_t paddleScale = max(PADDLE_SCALE_PERCENT * gameHeight / 100, 1);
  uint32_t ballScale   = max(BALL_SCALE_PERCENT   * gameHeight / 100, 1);
  // paddleScale = 1;
  ballScale = 1;

   // Create the paddle and ball shapes
   gameStatus.ballShape.CreateShape(ShapeTypeBall);
   gameStatus.leftPaddleShape.CreateShape(ShapeTypePaddle);
   gameStatus.rightPaddleShape.CreateShape(ShapeTypePaddle);

   // Copy the un-scaled shapes to their view coordinates from their world coordinates
   gameStatus.ballShape.CopyVerticesToView();
   gameStatus.leftPaddleShape.CopyVerticesToView();
   gameStatus.rightPaddleShape.CopyVerticesToView();

   // Now scale the shapes for the game world, the View will scale the view shape for the lasers
   gameStatus.ballShape.Scale(CoordsWorld, ballScale);
   gameStatus.leftPaddleShape.Scale(CoordsWorld, paddleScale);
   gameStatus.rightPaddleShape.Scale(CoordsWorld, paddleScale);

   // Paddles are at a fixed horizontal location
   int16_t  paddlePos = 8 * gameWidth / 9;
   gameStatus.leftPaddleShape.position.x  = -paddlePos;
   gameStatus.rightPaddleShape.position.x =  paddlePos;
   gameStatus.leftPaddleShape.position.y  =  0;
   gameStatus.rightPaddleShape.position.y =  0;

   // debug
   // gameStatus.ballShape.position.y = 0;

   // Set the limits on the paddleStatus, so we can't overdrive the paddles
   // The paddles should be the same size, so just use the left one as the benchmark
   int16_t  minLimit =  -(gameHeight / 2) + (gameStatus.leftPaddleShape.Height() / 2);
   int16_t  maxLimit =   (gameHeight / 2) - (gameStatus.leftPaddleShape.Height() / 2);

   leftPaddle.SetLimits(minLimit, maxLimit);
   rightPaddle.SetLimits(minLimit, maxLimit);
   
   // Now, ensure the status is in the vertical middle, since we moved our paddle shapes to the middle a few lines above
   leftPaddle.position = 0;
   rightPaddle.position = 0;

   // Serial.println("Done setting up game ready");
   // Serial.print("min and max limits: ");
   // Serial.print(minLimit);
   // Serial.print(", ");
   // Serial.println(maxLimit);
   // PrintLeftPaddleCoords();
   // PrintDisplayCoords();
}


void Engine::SetupGamePlay()
{
  Serial.println("Setup gameplay");
  delay(1000);
  
   int16_t twoThirdsHeight = 2 * gameHeight / 6;
   twoThirdsHeight = 0;
   
   // randomSeed(millis());

   // Randomly select top third or bottom third
   if(random(1) == 1)
   {
      // Select top 1/3 (0 + 2/3)
      // gameStatus.ballShape.position.y = twoThirdsHeight;
      gameStatus.ballShape.Move(CoordsWorld, 0, twoThirdsHeight);
   }
   else
   {
      // Select bottom 1/3 (0 - 2/3)
      // gameStatus.ballShape.position.y = -twoThirdsHeight;
      gameStatus.ballShape.Move(CoordsWorld, 0, -twoThirdsHeight);
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
   gameStatus.leftPaddleShape.Move(CoordsWorld, 0, (leftPaddle.position - gameStatus.leftPaddleShape.position.y));
   gameStatus.rightPaddleShape.Move(CoordsWorld, 0, (rightPaddle.position - gameStatus.rightPaddleShape.position.y));


   // Serial.println("\n\nBallshape position, ");
   // Serial.print(gameStatus.ballShape.position.x);
   // Serial.print(", ");
   // Serial.println(gameStatus.ballShape.position.y);
   PrintBallCoords();
   
   
   gameStatus.ballShape.Move(CoordsWorld, gameStatus.ballShape.vector.x, gameStatus.ballShape.vector.y);
  

   // Move the ball along it's trajectory
   // PrintDisplayCoords();
   // Serial.print(gameStatus.ballShape.position.x);
   // Serial.print(", ");
   // Serial.print(gameStatus.ballShape.position.y);
   // Serial.print(", ");
   // Serial.print(gameStatus.ballShape.vector.x);
   // Serial.print(", ");
   // Serial.println(gameStatus.ballShape.vector.y);

   Serial.println("\n\n");
   
   Serial.print("shape positions ");
   Serial.print(gameStatus.ballShape.position.x);
   Serial.print(", ");
   Serial.print(gameStatus.ballShape.position.y);
   Serial.print(", ");
   Serial.print(gameStatus.leftPaddleShape.position.x);
   Serial.print(", ");
   Serial.print(gameStatus.leftPaddleShape.position.y);
   Serial.print(", ");
   Serial.print(gameStatus.rightPaddleShape.position.x);
   Serial.print(", ");
   Serial.println(gameStatus.rightPaddleShape.position.y);
   
   // Check collision of the ball with the top or bottom
   if( (gameStatus.ballShape.CheckTop(-(gameHeight / 2), foundVertex)    ) ||
       (gameStatus.ballShape.CheckBottom( (gameHeight / 2), foundVertex) )    )
   {
      // Ball hit the top or bottom, so invert the y-component of the slope
      gameStatus.ballShape.vector.y *= -1;

      PlayWallSound();
   }

   // If the ball is traveling left, then check it for collision with the left paddle
   if(gameStatus.ballShape.vector.x < 0)
   {
      // If the ball's left-most vertex is between the highest and lowest paddle vertices...
      // Check if ball is at the right elevation
      if((gameStatus.ballShape.lowestVertex.y  <= gameStatus.leftPaddleShape.highestVertex.y) &&
         (gameStatus.ballShape.highestVertex.y >= gameStatus.leftPaddleShape.lowestVertex.y )    )
      {
         Serial.println("Checking for left side collision");
         Serial.println("paddle is at right elevation for bounce");
         // PrintBallCoords();
         // PrintLeftPaddleCoords();

         // And the it's beyond the paddle edge
         if((gameStatus.ballShape.leftMostVertex.x  <= gameStatus.leftPaddleShape.position.x) &&
            (gameStatus.ballShape.rightMostVertex.x >= gameStatus.leftPaddleShape.position.x)    )
         {
            Serial.println("BOUNCE");

            // Ball hit the left paddle so invert the x-component of the slope
            gameStatus.ballShape.vector.x *= -1;

            PlayPaddleSound();

            // TODO: take in velocity of the paddle to adjust slope of the ball
         }
      }

      // Check to see if the ball has reached the left edge
      if(gameStatus.ballShape.CheckLeft(-(gameWidth / 2), foundVertex))
      {
         Serial.println("Ball reached left edge");
         PrintDisplayCoords();
         Serial.print("Ball @ ");
         Serial.print(gameStatus.ballShape.position.x);
         Serial.print(", ");
         Serial.print(gameStatus.ballShape.position.y);
         Serial.println(", ");

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
         if(gameStatus.ballShape.CheckRight(gameStatus.rightPaddleShape.leftMostVertex.x, foundVertex) &&
            gameStatus.ballShape.CheckRight(gameStatus.rightPaddleShape.leftMostVertex.x, foundVertex)    )
         {
            // Ball hit the left paddle so invert the x-component of the slope
            gameStatus.ballShape.vector.x *= -1;

            PlayPaddleSound();

            // TODO: take in velocity of the paddle to adjust slope of the ball
         }
      }

      // Check to see if the ball has reached the right edge
      if(gameStatus.ballShape.CheckRight((gameWidth / 2), foundVertex))
      {
         Serial.println("Ball reached right edge");

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

            if( (gameStatus.leftPaddleScore  >= MAX_SCORE) ||
                (gameStatus.rightPaddleScore >= MAX_SCORE)    )
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

   // PrintButtonState();
}


void Engine::ChangeGameState(Model::GameState newState)
{
  delay(500); // debounce
  
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

void Engine::PrintDisplayCoords()
{
   Serial.print("Display Coords xmin, xmax, ymin, ymax: ");
   Serial.print(settings.display.xMin);
   Serial.print(", ");
   Serial.print(settings.display.xMax);
   Serial.print(", ");
   Serial.print(settings.display.yMin);
   Serial.print(", ");
   Serial.println(settings.display.yMax);
}

void Engine::PrintLeftPaddleCoords()
{
   Serial.print("LeftPaddle x, xmin, xmax, ymin, ymax = (");
   Serial.print(gameStatus.leftPaddleShape.position.x);
   Serial.print(", ");
   Serial.print(gameStatus.leftPaddleShape.leftMostVertex.x);
   Serial.print(", ");
   Serial.print(gameStatus.leftPaddleShape.rightMostVertex.x);
   Serial.print(", ");
   Serial.print(gameStatus.leftPaddleShape.lowestVertex.y);
   Serial.print(", ");
   Serial.print(gameStatus.leftPaddleShape.highestVertex.y);
   Serial.println(")");
}

void Engine::PrintRightPaddleCoords()
{
  Serial.print("RightPaddle x, xmin, xmax, ymin, ymax = (");
  Serial.print(gameStatus.rightPaddleShape.position.x);
  Serial.print(", ");
  Serial.print(gameStatus.rightPaddleShape.leftMostVertex.x);
  Serial.print(", ");
  Serial.print(gameStatus.rightPaddleShape.rightMostVertex.x);
  Serial.print(", ");
  Serial.print(gameStatus.rightPaddleShape.lowestVertex.y);
  Serial.print(", ");
  Serial.print(gameStatus.rightPaddleShape.highestVertex.y);
  Serial.println(")");
}

void Engine::PrintBallCoords()
{
   Serial.print("Ball xmin, xmax, ymin, ymax = (");
   Serial.print(gameStatus.ballShape.leftMostVertex.x);
   Serial.print(", ");
   Serial.print(gameStatus.ballShape.rightMostVertex.x);
   Serial.print(", ");
   Serial.print(gameStatus.ballShape.lowestVertex.y);
   Serial.print(", ");
   Serial.print(gameStatus.ballShape.highestVertex.y);
   Serial.println(")");
}
