
#include "Shapes.h"
#include "View.h"
#include "Arduino.h"

View::View(Circle&     _ball,
           Rectangle&  _leftPaddle,
           Rectangle&  _rightPaddle,
           LaserConf&  _leftLaserConf,
           LaserConf&  _rightLaserConf,
           LaserConf&  _ballLaserConf):
   ball(_ball),
   leftPaddle(_leftPaddle),
   rightPaddle(_rightPaddle),
   leftPaddleLaser(ShapeRectangle, _leftLaserConf),
   rightPaddleLaser(ShapeRectangle, _rightLaserConf),
   ballLaser(ShapeCircle, _ballLaserConf)
{
   leftPaddleLaser.SetRectangle(leftPaddle.width, leftPaddle.height);
   rightPaddleLaser.SetRectangle(rightPaddle.width, rightPaddle.height);
   ballLaser.SetCircle(ball.radius);
}


void View::Run(void)
{
   leftPaddleLaser.Draw(leftPaddle.x, leftPaddle.y);
   rightPaddleLaser.Draw(rightPaddle.x, rightPaddle.y);
   ballLaser.Draw(ball.x, ball.y);
}

/* TODO: Need help accessing the leftPaddle buttonPin
void View::Calibrate(void)
{
  // Use leftPaddle to point ballLaser at bottom left, top left, then top right corners.
  // Press leftPaddle button to set position of each corner.
  uint32_t xmin;
  uint32_t xmax;
  uint32_t ymin;
  uint32_t ymax;
  uint32_t hskew;
  uint32_t vskew;

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
