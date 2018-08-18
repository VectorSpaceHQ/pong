
#ifndef __engine_h__
#define __engine_h__

#include "Configs.h"
#include "Shapes.h"


class Engine
{
public:
   Engine(GameStatus&   _gameStatus,
          Circle&       _ball,
          Rectangle&    _leftPaddle,
          Rectangle&    _rightPaddle);

   void Run(void);

private:
   GameStatus& gameStatus;
   Circle&     ball;
   Rectangle&  leftPaddle;
   Rectangle&  rightPaddle;
};

#endif  // __engine_h__
