

#include "Pong.h"

Pong*  pongGame;


void setup() {
   Serial.begin(115200);
   Serial.println("Welcome to Giant Pong!");
   pongGame = new Pong();
}

void loop() {
  pongGame->Run();
}


