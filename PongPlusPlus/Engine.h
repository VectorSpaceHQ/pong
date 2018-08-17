

#ifndef __engine_h__
#define __engine_h__

#include "Shapes.h"


class Engine
{
public:
   Engine(Circle&      _ball,
          Rectangle&   _leftPaddle,
          Rectangle&   _rightPaddle);

   void Run(void);

private:
   Circle&     ball;
   Rectangle&  leftPaddle;
   Rectangle&  rightPaddle;

};

#endif  // __engine_h__
