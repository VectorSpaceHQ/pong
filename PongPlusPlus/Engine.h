
#ifndef __engine_h__
#define __engine_h__

#include "Model.h"

class Engine
{
public:
   Engine(Model::DisplaySettings&   _display,
          Model::GameStatus&        _gameStatus);

   void Run(void);

private:
   Model::DisplaySettings& display;
   Model::GameStatus&      gameStatus;

};

#endif  // __engine_h__
