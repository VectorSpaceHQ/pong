
#include <Arduino.h>

#include "Configs.h"
#include "Shapes.h"
#include "Pong.h"
#include "View.h"

Pong::Pong():

   leftPlayerLaser(
      3,    // Gimble X Pin
      5,    // Gimble Y Pin
      A2 ), // Laser Pin

  rightPlayerLaser(
      6,    // Gimble X Pin
      9,    // Gimble Y Pin
      A3 ),  // Laser Pin

   ballLaser(
      10,   // Gimble X Pin
      11,   // Gimble Y Pin
      A4 ), // Laser Pin     

   leftPaddleShape(
      10,   // X Coord
      0,    // Y Coord
      5,    // Width
      10 ), // Height

   rightPaddleShape(
      90,   // X Coord
      0,    // Y Coord
      5,    // Width
      10 ), // Height

   ballShape(
      90,   // X Coord
      0,    // Y Coord
      5 ),  // Radius

   view(
      ballShape,          // Ball Shape
      leftPaddleShape,    // Left Paddle Shape
      rightPaddleShape,   // Right Paddle Shape
      leftPlayerLaser,    // Left paddle laser configuration
      rightPlayerLaser,   // Right paddle laser configuration
      ballLaser )         // Ball laser configuration
{
}


void Pong::Start()
{
  
}


void Pong::Stop()
{
  
}


void Pong::Run()
{
  // TODO: Run Controller
  // TODO: Run Engine (model)
  view.Run();
}

