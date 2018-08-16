

#include "Pong.h"

Pong*  pongGame;


void setup() {
  pongGame = new Pong();
}

void loop() {
  pongGame->Run();
}


