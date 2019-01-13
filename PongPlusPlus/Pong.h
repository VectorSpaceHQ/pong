
#ifndef __pong_h__
#define __pong_h__


#include "Controller.h"
#include "Engine.h"
#include "HwConfig.h"
#include "Model.h"
#include "View.h"

class Pong
{
public:
   Pong();
   void Start();
   void Stop();
   void Run();

private:
   HwConfig                hardware;         // Hardware Configuration (i.e. which pins do what)
   Model::Settings         settings;         // Game settings
   Model::GameStatus       gameStatus;       // The current status of the game (set by the engine, read by the view)

   Controller              controller;       // The Controller runs the paddle drivers
   Engine                  engine;           // The Engine is the primary game engine
   View                    view;             // The View drives the lasers according to the current game status
};


#endif    // __pong_h__

