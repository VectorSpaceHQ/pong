/*
 * Engine.cpp
 *
 *  Created on: Aug 16, 2018
 *      Author: athiessen
 */

#include <Arduino.h>
#include <EEPROM.h>
#include <stdio.h>
#include <MatrixMath.h>

#include "Engine.h"
#include "Model.h"
#include "Sound.h"
#include "Timing.h"

// TODO: What should be the scale of the paddles?
#define PADDLE_SCALE_PERCENT        (2)            // Percent of the height of the paddle
#define BALL_SCALE_PERCENT          (5)            // Percent of the height of the ball

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
   gameWidth(0),
   cornerCounter(0)
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
         LoadSettings();
         SetupLaserCalibration();
         break;

      case Model::GameStateCalibrateLasers:
         RunLaserCalibration();
         break;

      case Model::GameStateCalibrateView:
         RunViewCalibration();
         
         CalcHomographyMatrix();
         break;

      case Model::GameStateCalibrateHomography:
         RunHomographyCalibration();
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

   settings.leftLaserCal.xOffset = 12;
   settings.leftLaserCal.yOffset = 141;
   settings.middleLaserCal.xOffset = -80;
   settings.middleLaserCal.yOffset = 180;
   settings.rightLaserCal.xOffset = -9;
   settings.rightLaserCal.yOffset = 167;

   settings.display.xMin = -143;
   settings.display.xMax = 150;
   settings.display.yMin = -64;
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


void Engine::RunHomographyCalibration()
{
  // ViewCalibrationButtonChange();
  
  switch(buttonState)
    {
    case ButtonStateNone: 
      {
	if (cornerCounter == 0) // top left
	  {
	    settings.middleLaserCal.topLeftX = leftPaddle.position;
	    settings.middleLaserCal.topLeftY = rightPaddle.position;
	  }
	else if (cornerCounter == 1) // top right
	  {
	    settings.middleLaserCal.topRightX = leftPaddle.position;
	    settings.middleLaserCal.topRightY = rightPaddle.position;
	  }
	else if (cornerCounter == 2) // bottom right
	  {
	    settings.middleLaserCal.botRightX = leftPaddle.position;
	    settings.middleLaserCal.botRightY = rightPaddle.position;
	  }
	else if (cornerCounter == 3)
	  {
	    settings.middleLaserCal.botLeftX = leftPaddle.position;
	    settings.middleLaserCal.botLeftY = rightPaddle.position;
	  }
      }
    case ButtonStateLeft:
      {
	if (cornerCounter == 0) // top left
	  {
	    settings.leftLaserCal.topLeftX = leftPaddle.position;
	    settings.leftLaserCal.topLeftY = rightPaddle.position;
	  }
	else if (cornerCounter == 1) // top right
	  {
	    settings.leftLaserCal.topRightX = leftPaddle.position;
	    settings.leftLaserCal.topRightY = rightPaddle.position;
	  }
	else if (cornerCounter == 2) // bot right
	  {
	    settings.leftLaserCal.botRightX = leftPaddle.position;
	    settings.leftLaserCal.botRightY = rightPaddle.position;
	  }
	else if (cornerCounter == 3)
	  {
	    settings.leftLaserCal.botLeftX = leftPaddle.position;
	    settings.leftLaserCal.botLeftY = rightPaddle.position;
	  }
      }
    case ButtonStateRight:
      {
	if (cornerCounter == 0) // top left
	  {
	    settings.rightLaserCal.topLeftX = leftPaddle.position;
	    settings.rightLaserCal.topLeftY = rightPaddle.position;
	  }
	else if (cornerCounter == 1) // top right
	  {
	    settings.rightLaserCal.topRightX = leftPaddle.position;
	    settings.rightLaserCal.topRightY = rightPaddle.position;
	  }
	else if (cornerCounter == 2) // bot right
	  {
	    settings.rightLaserCal.botRightX = leftPaddle.position;
	    settings.rightLaserCal.botRightY = rightPaddle.position;
	  }
	else if (cornerCounter == 3)
	  {
	    settings.rightLaserCal.botLeftX = leftPaddle.position;
	    settings.rightLaserCal.botLeftY = rightPaddle.position;
	  }
      }
    case ButtonStateBoth:
      {
	cornerCounter++;
      }
    
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
   uint32_t paddleScale = PADDLE_SCALE_PERCENT * gameHeight / 100;
   uint32_t ballScale   = BALL_SCALE_PERCENT   * gameHeight / 100;

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

   PrintDisplayCoords();

   // Paddles are at a fixed horizontal location
   int16_t  oneQuarter = gameWidth / 4;
   gameStatus.leftPaddleShape.position.x  = -oneQuarter;
   gameStatus.rightPaddleShape.position.x =  oneQuarter;
   gameStatus.leftPaddleShape.position.y  =  0;
   gameStatus.rightPaddleShape.position.y =  0;

   // Set the limits on the paddleStatus, so we can't overdrive the paddles
   // The paddles should be the same size, so just use the left one as the benchmark
   int16_t  minLimit =  -(gameHeight / 2) + (gameStatus.leftPaddleShape.Height() / 2);
   int16_t  maxLimit =   (gameHeight / 2) - (gameStatus.leftPaddleShape.Height() / 2);

   leftPaddle.SetLimits(minLimit, maxLimit);
   rightPaddle.SetLimits(minLimit, maxLimit);

   // Now, ensure the status is in the vertical middle, since we moved our paddle shapes to the middle a few lines above
   leftPaddle.position = 0;
   rightPaddle.position = 0;
}


void Engine::SetupGamePlay()
{
   int16_t twoThirdsHeight = 2 * gameHeight / 6;

   randomSeed(micros());

   // Randomly select top third or bottom third
   if(random(1) == 1)
   {
      // Select top 1/3 (0 + 2/3)
      gameStatus.ballShape.position.y = twoThirdsHeight;
   }
   else
   {
      // Select bottom 1/3 (0 - 2/3)
      gameStatus.ballShape.position.y = -twoThirdsHeight;
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

   // Move the ball along it's trajectory
   // PrintDisplayCoords();
   // Serial.print(gameStatus.ballShape.position.x);
   // Serial.print(", ");
   // Serial.print(gameStatus.ballShape.position.y);
   // Serial.print(", ");
   // Serial.print(gameStatus.ballShape.vector.x);
   // Serial.print(", ");
   // Serial.println(gameStatus.ballShape.vector.y);
   
   gameStatus.ballShape.Move(CoordsWorld, gameStatus.ballShape.vector.x, gameStatus.ballShape.vector.y);

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
         PrintBallCoords();
         PrintLeftPaddleCoords();

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

   delay(200); // button debounce
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
   Serial.print("x = (");
   Serial.print(settings.display.xMin);
   Serial.print(", ");
   Serial.print(settings.display.xMax);
   Serial.print("), y = (");
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

void Engine::CalcHomographyMatrix(){
  // game coords, starting bottom left, go clockwise
  int32_t x1 = settings.display.xMin;
  int32_t x2 = x1;
  int32_t x3 = settings.display.xMax;
  int32_t x4 = x3;
  int32_t y1 = settings.display.yMin;
  int32_t y2 = settings.display.yMax;
  int32_t y3 = y2;
  int32_t y4 = settings.display.yMin;

  // servo coords
  int32_t u1 = settings.middleLaserCal.botLeftX; 
  int32_t u2 = settings.middleLaserCal.topLeftX;
  int32_t u3 = settings.middleLaserCal.topRightX;
  int32_t u4 = settings.middleLaserCal.botRightX;
  int32_t v1 = settings.middleLaserCal.botLeftY;
  int32_t v2 = settings.middleLaserCal.topLeftY;
  int32_t v3 = settings.middleLaserCal.topRightY;
  int32_t v4 = settings.middleLaserCal.botRightY;

  Serial.println(x1);
  Serial.println(x2);
  Serial.println(x3);
  Serial.println(x4);  
  Serial.println(y1);
  Serial.println(y2);
  Serial.println(y3);
  Serial.println(y4);
  Serial.println(u1);
  Serial.println(u2);
  Serial.println(u3);
  Serial.println(u4);
  Serial.println(v1);
  Serial.println(v2);
  Serial.println(v3);
  Serial.println(v4);
  

  

  mtx_type p[9][9];
  mtx_type q[9];
  mtx_type h[9];
  mtx_type p_T[9][9];

  q[0] = 0;
  q[1] = 0;
  q[2] = 0;
  q[3] = 0;
  q[4] = 0;
  q[5] = 0;
  q[6] = 0;
  q[7] = 0;
  q[8] = 1;

  p[0][0] = -x1;
  p[0][1] = -y1;
  p[0][2] = -1;
  p[0][3] = 0;
  p[0][4] = 0;
  p[0][5] = 0;
  p[0][6] = x1*u1;
  p[0][7] = y1*u1;
  p[0][8] = u1;

  p[1][0] = 0;
  p[1][1] = 0;
  p[1][2] = 0;
  p[1][3] = -x1;
  p[1][4] = -y1;
  p[1][5] = -1;
  p[1][6] = x1*v1;
  p[1][7] = y1*v1;
  p[1][8] = v1;

  p[2][0] = -x2;
  p[2][1] = -y2;
  p[2][2] = -1;
  p[2][3] = 0;
  p[2][4] = 0;
  p[2][5] = 0;
  p[2][6] = x2*u2;
  p[2][7] = y2*u2;
  p[2][8] = u2;

  p[3][0] = 0;
  p[3][1] = 0;
  p[3][2] = 0;
  p[3][3] = -x2;
  p[3][4] = -y2;
  p[3][5] = -1;
  p[3][6] = x2*v2;
  p[3][7] = y2*v2;
  p[3][8] = v2;

  p[4][0] = -x3;
  p[4][1] = -y3;
  p[4][2] = -1;
  p[4][3] = 0;
  p[4][4] = 0;
  p[4][5] = 0;
  p[4][6] = x3*u3;
  p[4][7] = y3*u3;
  p[4][8] = u3;

  p[5][0] = 0;
  p[5][1] = 0;
  p[5][2] = 0;
  p[5][3] = -x3;
  p[5][4] = -y3;
  p[5][5] = -1;
  p[5][6] = x3*v3;
  p[5][7] = y3*v3;
  p[5][8] = v3;

  p[6][0] = -x4;
  p[6][1] = -y4;
  p[6][2] = -1;
  p[6][3] = 0;
  p[6][4] = 0;
  p[6][5] = 0;
  p[6][6] = x4*u4;
  p[6][7] = y4*u4;
  p[6][8] = u4;

  p[7][0] = 0;
  p[7][1] = 0;
  p[7][2] = 0;
  p[7][3] = -x4;
  p[7][4] = -y4;
  p[7][5] = -1;
  p[7][6] = x4*v4;
  p[7][7] = y4*v4;
  p[7][8] = v4;

  p[8][0] = 0;
  p[8][1] = 0;
  p[8][2] = 0;
  p[8][3] = 0;
  p[8][4] = 0;
  p[8][5] = 0;
  p[8][6] = 0;
  p[8][7] = 0;
  p[8][8] = 1;


  Matrix.Transpose((mtx_type*)p, 9, 9, (mtx_type*) p_T);

  mtx_type C[9][9];
  Matrix.Multiply((mtx_type*)p_T, (mtx_type*)p, 9, 9, 9, (mtx_type*)C);

  mtx_type D[9][9];
  Matrix.Multiply((mtx_type*)p_T, (mtx_type*)q, 9, 9, 1, (mtx_type*)D);


  Matrix.Print((mtx_type*)p, 9, 9, "p");
  Matrix.Print((mtx_type*)p_T, 9, 9, "p_T");
  Matrix.Print((mtx_type*)q, 9, 1, "q");
  Matrix.Print((mtx_type*)C, 9, 9, "C");

  
  Matrix.Invert((mtx_type*)C, 9); // wow this library is bad


  
  mtx_type E[9][1];
  Matrix.Multiply((mtx_type*)C, (mtx_type*)D, 9, 9, 1, (mtx_type*)E);

  // Serial.print(p);

  Matrix.Print((mtx_type*)C, 9, 1, "inverted C");
  Matrix.Print((mtx_type*)D, 9, 1, "D");
  Matrix.Print((mtx_type*)E, 9, 1, "E");


  mtx_type H[3][3];
  H[0][0] = E[0][0];
  H[0][1] = E[1][0];
  H[0][2] = E[2][0];
  H[1][0] = E[3][0];
  H[1][1] = E[4][0];
  H[1][2] = E[5][0];
  H[2][0] = E[6][0];
  H[2][1] = E[7][0];
  H[2][2] = E[8][0];

  Matrix.Print((mtx_type*)H, 3, 3, "H");


  int x = 30;
  int y = 40;
  
  mtx_type a[3][1];
  a[0][0] = x;
  a[1][0] = y;
  a[2][0] = 1;

  mtx_type solution[3][1];
  Matrix.Multiply((mtx_type*)H, (mtx_type*)a, 3, 3, 1, (mtx_type*)solution);
  Matrix.Print((mtx_type*)solution, 3, 1, "solution");
  delay(10000);

  // Matrix.Multiply(p_T, p);
  // Matrix.Print((mtx_type*)w, N, 1, "w");


  // h = Matrix.invert(Matrix.transpose(p,9,9) * p) * (Matrix.transpose(p,9,9) * q);
}
