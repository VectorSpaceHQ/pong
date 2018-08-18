
#include <Arduino.h>

#include "Configs.h"
#include "Controller.h"
#include "Engine.h"
#include "Shapes.h"
#include "Pong.h"
#include "View.h"

Pong::Pong():
   display(),
   gameStatus(),

   leftPlayerLaser(
      3,    // Gimble X Pin
      5,    // Gimble Y Pin
      A2 ), // Laser Pin

  rightPlayerLaser(
      6,    // Gimble X Pin
      9,    // Gimble Y Pin
      A3 ), // Laser Pin

   ballLaser(
      10,   // Gimble X Pin
      11,   // Gimble Y Pin
      A4 ), // Laser Pin     

   leftPaddle(
      7,    // Up Pin
      8,    // Down Pin
      A0 ), // Button Pin

   rightPaddle(
      2,    // Up Pin
      4,    // Down Pin
      A1 ), // Button Pin

   engine(display,         // Display Settings
          gameStatus),     // Game Status

   view(
      display,             // Display Settings
      gameStatus,          // Game Status
      leftPlayerLaser,     // Left paddle laser configuration
      rightPlayerLaser,    // Right paddle laser configuration
      ballLaser ),         // Ball laser configuration

   controller(leftPaddle,  // Left Paddle Config
              rightPaddle) // Right Paddle Config
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
   controller.Run();    // Update paddle HW info
   engine.Run();        // Run game engine
   view.Run();          // Draw the screen
}
