
#include "Shapes.h"
#include "View.h"

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

