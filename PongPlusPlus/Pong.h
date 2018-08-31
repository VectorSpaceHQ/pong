
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

   Controller              controller;
   Engine                  engine;
   View                    view;
};


#endif    // __pong_h__

