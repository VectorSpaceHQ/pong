
#ifndef __pong_h__
#define __pong_h__


#include "Configs.h"
#include "Controller.h"
#include "Engine.h"
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
   Model::DisplaySettings  display;
   Model::GameStatus       gameStatus;

   LaserConf               leftPlayerLaser;
   LaserConf               rightPlayerLaser;
   LaserConf               ballLaser;

   PaddleConf              leftPaddle;
   PaddleConf              rightPaddle;

   Engine                  engine;
   View                    view;
   Controller              controller;
};


#endif    // __pong_h__

