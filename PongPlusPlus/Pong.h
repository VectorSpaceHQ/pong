
#ifndef __pong_h__
#define __pong_h__


#include "Configs.h"
#include "Controller.h"
#include "Engine.h"
#include "Shapes.h"
#include "View.h"

class Pong
{
public:
   Pong();
   void Start();
   void Stop();
   void Run();

private:
   LaserConf   leftPlayerLaser;
   LaserConf   rightPlayerLaser;
   LaserConf   ballLaser;

   PaddleConf  leftPaddle;
   PaddleConf  rightPaddle;

   Rectangle   leftPaddleShape;
   Rectangle   rightPaddleShape;
   Circle      ballShape;

   Engine      engine;
   View        view;
   Controller  controller;
};


#endif    // __pong_h__

