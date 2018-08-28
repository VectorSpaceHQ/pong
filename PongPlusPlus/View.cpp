
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

/*
   gameStatus.ballShape.CreateShape(2);
   gameStatus.ballShape.Scale(-1);        // This laser needs its shape inverted
   ballLaser.UpdateShape(20);
   ballLaser.SetPosition(0, 0);

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
   // TODO: What to display for calibration? Perhaps have one laser draw a rectangle for the given size?
}


void View::DisplayGamePlay(void)
{
}


void View::DisplayScore(void)
{
}


/* TODO: Need help accessing the leftPaddle buttonPin
 * ADT: Only the Engine will have access to controller information.
 *      The engine will update values in the Model DisplaySettings struct
 *      The View will be able to read the Display settings and update it's display accordingly.
void View::Calibrate(void)
{
  // Use leftPaddle to point ballLaser at bottom left, top left, then top right corners.
  // Press leftPaddle button to set position of each corner.

  // Instead of pointing the laser at corners, what if the laser drew a rectangle for the size of the display,
  // and we adjust the rectangle to match the shape of the building?


  // Bottom left
  while (digitalRead(leftPaddle.buttonPin) == HIGH){
    ballLaser.Draw(ball.x, ball.y);
  }
  xmin = ball.x;
  ymin = ball.y;
  delay(1000);

  // Top left
  while (digitalRead(leftPaddle.buttonPin) == HIGH){
    ballLaser.Draw(ball.x, ball.y);
  }
  hskew = ball.x - xmin;
  ymax = ball.y;
  delay(1000);

  // Top right
  while (digitalRead(leftPaddle.buttonPin) == HIGH){
    ballLaser.Draw(ball.x, ball.y);
  }
  vskew = ball.y - ymax;
  xmax = ball.x;

  leftPaddleLaser.SetBounds(xmin, xmax, ymin, ymax, hskew, vskew);
  rightPaddleLaser.SetBounds(xmin, xmax, ymin, ymax, hskew, vskew);
  ballLaser.SetBounds(xmin, xmax, ymin, ymax, hskew, vskew);
}
*/
