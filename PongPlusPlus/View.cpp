
#include "Shapes.h"
#include "View.h"

View::View( Circle&     _ball,
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
}

