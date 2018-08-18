/*
 * Engine.cpp
 *
 *  Created on: Aug 16, 2018
 *      Author: athiessen
 */

#include "Engine.h"
#include "Shapes.h"

Engine::Engine(GameStatus&    _gameStatus,
               Circle&        _ball,
               Rectangle&     _leftPaddle,
               Rectangle&     _rightPaddle):
   gameStatus(_gameStatus),
   ball(_ball),
   leftPaddle(_leftPaddle),
   rightPaddle(_rightPaddle)
{
}


void Engine::Run(void)
{
}
