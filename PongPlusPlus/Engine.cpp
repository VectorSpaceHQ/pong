/*
 * Engine.cpp
 *
 *  Created on: Aug 16, 2018
 *      Author: athiessen
 */

#include "Engine.h"
#include "Model.h"


Engine::Engine(Model::DisplaySettings&    _display,
               Model::GameStatus&         _gameStatus,
               PaddleStatus&             _leftPaddle,
               PaddleStatus&             _rightPaddle):
   display(_display),
   gameStatus(_gameStatus),
   leftPaddle(_leftPaddle),
   rightPaddle(_rightPaddle)
{
}


void Engine::Run(void)
{
}
