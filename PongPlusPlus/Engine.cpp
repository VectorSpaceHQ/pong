/*
 * Engine.cpp
 *
 *  Created on: Aug 16, 2018
 *      Author: athiessen
 */

#include "Engine.h"
#include "Model.h"


Engine::Engine(Model::DisplaySettings& _display,
               Model::GameStatus&      _gameStatus):
   display(_display),
   gameStatus(_gameStatus)
{
}


void Engine::Run(void)
{
}
