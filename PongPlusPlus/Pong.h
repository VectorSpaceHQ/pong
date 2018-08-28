
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
   HwConfig                hardware;
   Model::Settings         settings;
   Model::GameStatus       gameStatus;

   Engine                  engine;
   View                    view;
   Controller              controller;
};


#endif    // __pong_h__

