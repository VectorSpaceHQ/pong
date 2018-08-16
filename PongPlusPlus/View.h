
#ifndef __view_h__
#define __view_h__


#include "Shapes.h"
#include "LaserCtrl.h"


class View
{
  public:
    View(Circle&      _ball,
         Rectangle&   _leftPaddle,
         Rectangle&   _rightPaddle,
         LaserConf&   _leftLaserConf,
         LaserConf&   _rightLaserConf,
         LaserConf&   _ballLaserConf);

    void  Run(void);
    
  private:
    Circle&     ball;
    Rectangle&  leftPaddle;
    Rectangle&  rightPaddle;

    LaserCtrl   leftPaddleLaser;
    LaserCtrl   rightPaddleLaser;
    LaserCtrl   ballLaser;
};


#endif    // __view_h__

