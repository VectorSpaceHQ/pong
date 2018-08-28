
#include <Arduino.h>

#include "LaserCtrl.h"
#include "Model.h"
#include "ScheduledInterval.h"
#include "Shape.h"
#include "Timing.h"
#include "View.h"

// TODO: After we figure out game resolution and variable display, these may need to be configurable
#define  SCORE_LEFT_X   (200)
#define  SCORE_LEFT_Y   (300)
#define  SCORE_RIGHT_X  (600)
#define  SCORE_RIGHT_Y  (300)


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
   // Wait for our lasers to get to their initial position
   delay(100);

   // Set Laser orientation
   //settings.leftLaserCal.xOrientation = -1;
   settings.leftLaserCal.yOrientation = -1;
   //settings.middleLaserCal.xOrientation = -1;
   settings.middleLaserCal.yOrientation = -1;
   settings.middleLaserCal.xOrientation = -1;

/*
   gameStatus.ballShape.CreateShape(2);
   ballLaser.UpdateShape(20, true);
   ballLaser.SetPosition(0, 0);
   ballLaser.Start();

   gameStatus.leftPaddleShape.CreateShape(1);
   leftPaddleLaser.UpdateShape(20);
   leftPaddleLaser.SetPosition(0, 0);

   gameStatus.rightPaddleShape.CreateShape(0);
   gameStatus.rightPaddleShape.Scale(-1);        // This laser needs its shape inverted
   rightPaddleLaser.UpdateShape(20);
   rightPaddleLaser.SetPosition(0, 0);

   // Start the lasers
   ballLaser.Start();
   leftPaddleLaser.Start();
   rightPaddleLaser.Start();
*/
}


void View::Update(void)
{
/*
   ballLaser.Run();
   leftPaddleLaser.Run();
   rightPaddleLaser.Run();
*/

   switch(gameStatus.gameState)
   {
      case Model::GameStateCalibrateLasers:
         DisplayLaserCalibration();
         break;

      case Model::GameStateCalibrateView:
         DisplayViewCalibration();
         break;

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


void View::DisplayLaserCalibration(void)
{
   // Turn the lasers on
   leftPaddleLaser.On();
   ballLaser.On();
   rightPaddleLaser.On();

   // Set their position to the centerpoint
   leftPaddleLaser.Move(0, 0);
   ballLaser.Move(0, 0);
   rightPaddleLaser.Move(0, 0);
}


void View::DisplayViewCalibration(void)
{
   if(gameStatus.gameStateChanged)
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

      // Reset gameStateChanged
      gameStatus.gameStateChanged = false;

      // Set up the ball shape to be a horizontal line in the center
      gameStatus.ballShape.Reset();
      gameStatus.ballShape.AddVertex(settings.display.xMin, 0, true);
      gameStatus.ballShape.AddVertex(settings.display.xMax, 0, true);
      ballLaser.UpdateShape(1, true);
      ballLaser.SetPosition(0, 0);
      ballLaser.Start();

      // Setup the left laser to be a vertical line  on the left of the screen
      gameStatus.leftPaddleShape.Reset();
      gameStatus.leftPaddleShape.AddVertex(0, settings.display.yMin, true);   // Top
      gameStatus.leftPaddleShape.AddVertex(0, settings.display.yMax, true);   // Bottom
      leftPaddleLaser.UpdateShape(1, true);
      leftPaddleLaser.SetPosition(settings.display.xMin, 0);   // far left
      leftPaddleLaser.Start();

      // Setup the right laser to be a vertical line  on the right of the screen
      gameStatus.rightPaddleShape.Reset();
      gameStatus.rightPaddleShape.AddVertex(0, settings.display.yMin, true);   // Top
      gameStatus.rightPaddleShape.AddVertex(0, settings.display.yMax, true);   // Bottom
      rightPaddleLaser.UpdateShape(1, true);
      rightPaddleLaser.SetPosition(settings.display.xMax, 0);   // far right
      rightPaddleLaser.Start();
   }

   ballLaser.Run();
   leftPaddleLaser.Run();
   rightPaddleLaser.Run();
}


void View::DisplayGamePlay(void)
{
}


void View::DisplayScore(void)
{
}
