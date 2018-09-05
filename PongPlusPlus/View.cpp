
#include <Arduino.h>

#include "LaserCtrl.h"
#include "Model.h"
#include "ScheduledInterval.h"
#include "Shape.h"
#include "Timing.h"
#include "View.h"


#define SCORE_SCALE_PERCENT         (10)           // Percent of the height of the score


View::View(Model::Settings&         _settings,
           Model::GameStatus&       _gameStatus,
           LaserConf&               _leftLaserConf,
           LaserConf&               _rightLaserConf,
           LaserConf&               _ballLaserConf):
   ScheduledInterval(VIEW_LOOP_INTERVAL),
   settings(_settings),
   gameStatus(_gameStatus),
   leftPaddleLaser(_leftLaserConf,
                   settings.leftLaserCal,
                   gameStatus.leftPaddleShape,
                   "LeftPaddle"),
   rightPaddleLaser(_rightLaserConf,
                    settings.rightLaserCal,
                    gameStatus.rightPaddleShape,
                    "RightPaddle"),
   ballLaser(_ballLaserConf,
             settings.middleLaserCal,
             gameStatus.ballShape,
             "Ball")
{
   Serial.println("View Up");

   // Wait for our lasers to get to their initial position
   delay(100);

   // Set Laser orientation
   settings.leftLaserCal.xOrientation = -1;
   settings.leftLaserCal.yOrientation = -1;

   settings.middleLaserCal.xOrientation = -1;
   settings.middleLaserCal.yOrientation = -1;

   settings.rightLaserCal.xOrientation = -1;
}


/******************************************************************************
 * Main View Loop
 ******************************************************************************/
void View::Update(void)
{
   CheckGameStateChange();

   switch(gameStatus.gameState)
   {
      case Model::GameStateCalibrateLasers:
         DisplayLaserCalibration();
         break;

      case Model::GameStateCalibrateView:
         DisplayViewCalibration();
         break;

      // Game Ready and Play are almost the same for the view
      // Only, in ready state, the ball is not in play and is not shown
      case Model::GameStateReady:
      case Model::GameStatePlay:
         DisplayGamePlay();
         break;

      case Model::GameStateGameOver:
      default:
         DisplayScore();
         // TODO: Maybe the Default should be the Vector Space logo?  :-)
         break;
   }
}


/***
 * Checks to see if the game state has changed since the last time through.
 */
void View::CheckGameStateChange(void)
{
   // If so, then we need to run the setup for the new state
   if(gameStatus.gameStateChanged)
   {
      // Reset gameStateChanged
      gameStatus.gameStateChanged = false;

      switch(gameStatus.gameState)
      {
         case Model::GameStateCalibrateLasers:
            SetupLaserCalibration();
            break;

         case Model::GameStateCalibrateView:
            SetupViewCalibration();
            break;

         case Model::GameStateReady:
            SetupGameReady();
            break;

         case Model::GameStatePlay:
            SetupGamePlay();
            break;

         case Model::GameStateGameOver:
            SetupGameOver();
            break;

         default:
            SetupGameReady();
            // TODO: Maybe the Default should be the Vector Space logo?  :-)
            break;
      }
   }
}


/******************************************************************************
 * Laser Calibration
 ******************************************************************************/
void View::SetupLaserCalibration(void)
{
   // Stop the lasers from running through their shapes, so they stay in a single position
   leftPaddleLaser.Stop();
   ballLaser.Stop();
   rightPaddleLaser.Stop();

   // Turn the lasers on
   leftPaddleLaser.On();
   ballLaser.On();
   rightPaddleLaser.On();
}


void View::DisplayLaserCalibration(void)
{
   // Manually set laser position to the center point and let the offsets do the work.
   // We do this in the main loop so the center point gets re-calculated
   leftPaddleLaser.Move(0, 0);
   ballLaser.Move(0, 0);
   rightPaddleLaser.Move(0, 0);
}


/******************************************************************************
 * View Calibration
 ******************************************************************************/
void View::SetupViewCalibration(void)
{
   Serial.print("View X-Y( ");
   Serial.print(settings.display.xMin);
   Serial.print(" - ");
   Serial.print(settings.display.xMax);
   Serial.print(", ");
   Serial.print(settings.display.yMin);
   Serial.print(" - ");
   Serial.print(settings.display.yMax);
   Serial.println(" )");

   // Set up the ball shape to be a horizontal line at the bottom
   gameStatus.ballShape.Reset();
   gameStatus.ballShape.AddVertex(settings.display.xMin, settings.display.yMin, true);
   gameStatus.ballShape.AddVertex(settings.display.xMax, settings.display.yMin, true);
   ballLaser.UpdateShape(1, true, true);
   ballLaser.SetPosition(0, 0);
   ballLaser.Start();

   // Setup the left laser to be a vertical line  on the left of the screen
   gameStatus.leftPaddleShape.Reset();
   gameStatus.leftPaddleShape.AddVertex(settings.display.xMin + (settings.display.xMax - settings.display.xMin) / 4, settings.display.yMin, true);   // Top
   gameStatus.leftPaddleShape.AddVertex(settings.display.xMin + (settings.display.xMax - settings.display.xMin) / 4, settings.display.yMax, true);   // Bottom
   leftPaddleLaser.UpdateShape(1, true, true);
   leftPaddleLaser.SetPosition(settings.display.xMin, 0);   // far left
   leftPaddleLaser.Start();

   // Setup the right laser to be a vertical line  on the right of the screen
   gameStatus.rightPaddleShape.Reset();
   gameStatus.rightPaddleShape.AddVertex(settings.display.xMin + 3 * (settings.display.xMax - settings.display.xMin) / 4, settings.display.yMin, true);   // Top
   gameStatus.rightPaddleShape.AddVertex(settings.display.xMin + 3 * (settings.display.xMax - settings.display.xMin) / 4, settings.display.yMax, true);   // Bottom
   rightPaddleLaser.UpdateShape(1, true, true);
   rightPaddleLaser.SetPosition(settings.display.xMax, 0);   // far right
   rightPaddleLaser.Start();
}


void View::DisplayViewCalibration(void)
{
   // If the view settings have changed, we need to update the size/location of the shapes
   if(gameStatus.viewSettingsChanged)
   {
      // Reset gameStateChanged
      gameStatus.viewSettingsChanged = false;

      // Now setup the calibration with the updated settings
      SetupViewCalibration();
   }

   // Run the lasers throught their shape
   ballLaser.Run();
   leftPaddleLaser.Run();
   rightPaddleLaser.Run();
}


/******************************************************************************
 * Game Ready/Play -- Almost the same
 ******************************************************************************/
void View::SetupGameReady(void)
{
   // For now, set the Lasers to the same scale as the game
   uint32_t paddleScale = gameStatus.leftPaddleShape.scale;
   uint32_t ballScale = gameStatus.ballShape.scale;

   // Stop the ball laser from running its shape and turn it off
   // Since it's not shown in the ready state
   ballLaser.UpdateShape(ballScale, true, false);
   ballLaser.Stop();
   ballLaser.Off();
   
   // Create the left paddle shape
   leftPaddleLaser.UpdateShape(paddleScale, true, false);
   leftPaddleLaser.Start();

   // Create the right paddle shape
   rightPaddleLaser.UpdateShape(paddleScale, true, false);
   rightPaddleLaser.Start();
}


void View::SetupGamePlay(void)
{
   // The shapes for game play are now configured in the engine
   // We can just use the same shape settings as Game Ready, so no need to configure anything here.

   // Just make sure the lasers are started to run through their shapes
   ballLaser.Start();
   leftPaddleLaser.Start();
   rightPaddleLaser.Start();
}


void View::DisplayGamePlay(void)
{
   // Move the lasers
   leftPaddleLaser.SetPosition(gameStatus.leftPaddleShape.position.x,
                               gameStatus.leftPaddleShape.position.y);

   rightPaddleLaser.SetPosition(gameStatus.rightPaddleShape.position.x,
                                gameStatus.rightPaddleShape.position.y);

   ballLaser.SetPosition(gameStatus.ballShape.position.x,
                         gameStatus.ballShape.position.y);

   // Run the lasers through their shapes
   ballLaser.Run();
   leftPaddleLaser.Run();
   rightPaddleLaser.Run();

   // Serial.print("View DisplayGamePlay ball x,y = ");
   // Serial.print(gameStatus.ballShape.position.x);
   // Serial.print(", ");
   // Serial.println(gameStatus.ballShape.position.y);
   
}


/******************************************************************************
 * Game Over
 ******************************************************************************/
void View::SetupGameOver(void)
{
   uint32_t    paddleScale    = SCORE_SCALE_PERCENT * (settings.display.yMax - settings.display.yMin)  / 100;
   CoordType   leftScoreLoc   = settings.display.xMin + (settings.display.xMax - settings.display.xMin) / 4;
   CoordType   rightScoreLoc  = settings.display.xMin + 3 * (settings.display.xMax - settings.display.xMin) / 4;

   // Set up the ball shape to be a hyphen
   gameStatus.ballShape.Reset();
   gameStatus.ballShape.AddVertex(-25, 0, true);
   gameStatus.ballShape.AddVertex( 25, 0, true);
   ballLaser.UpdateShape(1, true, true);
   ballLaser.SetPosition((settings.display.xMin + settings.display.xMax) / 2,
                         (settings.display.yMin + settings.display.yMax) / 2);
   ballLaser.Start();

   // Use the left paddle to display left player score
   gameStatus.leftPaddleShape.CreateShape(gameStatus.leftPaddleScore);
   leftPaddleLaser.UpdateShape(paddleScale, true, true);
   leftPaddleLaser.SetPosition(leftScoreLoc, (settings.display.yMin + settings.display.yMax) / 2);
   leftPaddleLaser.Start();

   // Use the left paddle to display right player score
   gameStatus.rightPaddleShape.CreateShape(gameStatus.rightPaddleScore);
   rightPaddleLaser.UpdateShape(paddleScale, true, true);
   rightPaddleLaser.SetPosition(rightScoreLoc, (settings.display.yMin + settings.display.yMax) / 2);
   rightPaddleLaser.Start();
}


void View::DisplayScore(void)
{
   // Just let the lasers run through their shapes
   ballLaser.Run();
   leftPaddleLaser.Run();
   rightPaddleLaser.Run();
}

