
#ifndef __pong_h__
#define __pong_h__


#include "Configs.h"
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
    LaserConf leftPlayerLaser;
    LaserConf rightPlayerLaser;
    LaserConf ballLaser;

    Rectangle leftPaddleShape;
    Rectangle rightPaddleShape;
    Circle    ballShape;

    View  view;
};


#endif    // __pong_h__

