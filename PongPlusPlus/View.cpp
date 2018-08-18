
#include "LaserCtrl.h"
#include "Model.h"
#include "View.h"

// TODO: After we figure out game resolution and variable display, these may need to be configurable
#define  SCORE_LEFT_X   (200)
#define  SCORE_LEFT_Y   (300)
#define  SCORE_RIGHT_X  (600)
#define  SCORE_RIGHT_Y  (300)


View::View(Model::DisplaySettings&  _display,
           Model::GameStatus&       _gameStatus,
           LaserConf&               _leftLaserConf,
           LaserConf&               _rightLaserConf,
           LaserConf&               _ballLaserConf):
   display(_display),
   gameStatus(_gameStatus),
   leftPaddleLaser(ShapeRectangle, _leftLaserConf),
   rightPaddleLaser(ShapeRectangle, _rightLaserConf),
   ballLaser(ShapeCircle, _ballLaserConf)
{
   leftPaddleLaser.SetRectangle(gameStatus.leftPaddleShape.width,
                                gameStatus.leftPaddleShape.height);
   rightPaddleLaser.SetRectangle(gameStatus.rightPaddleShape.width,
                                 gameStatus.rightPaddleShape.height);
   ballLaser.SetCircle(gameStatus.ballShape.radius);
}


void View::Run(void)
{
   switch(gameStatus.gameState)
   {
      case Model::GameStateCalibrate:
         DisplayCalibration();
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


void View::DisplayCalibration(void)
{
   // TODO: What to display for calibration? Perhaps have one laser draw a rectangle for the given size?
}


void View::DisplayGamePlay(void)
{
   leftPaddleLaser.Draw(gameStatus.leftPaddleShape.x, gameStatus.leftPaddleShape.y);
   rightPaddleLaser.Draw(gameStatus.rightPaddleShape.x, gameStatus.rightPaddleShape.y);
   ballLaser.Draw(gameStatus.ballShape.x, gameStatus.ballShape.y);
}


void View::DisplayScore(void)
{
   leftPaddleLaser.DrawScore(SCORE_LEFT_X, SCORE_LEFT_Y, gameStatus.leftPaddleScore);
   rightPaddleLaser.DrawScore(SCORE_RIGHT_X, SCORE_RIGHT_Y, gameStatus.rightPaddleScore);
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
