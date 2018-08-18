
#include "Controller.h"
#include "Engine.h"
#include "HwConfig.h"
#include "Shapes.h"
#include "Pong.h"
#include "View.h"


Pong::Pong():
   hardware(),
   display(),
   gameStatus(),

   engine(display,                  // Display Settings
          gameStatus),              // Game Status

   view(
      display,                      // Display Settings
      gameStatus,                   // Game Status
      hardware.leftPlayerLaser,     // Left paddle laser configuration
      hardware.rightPlayerLaser,    // Right paddle laser configuration
      hardware.ballLaser ),         // Ball laser configuration

   controller(hardware.leftPaddle,  // Left paddle configuration
              hardware.rightPaddle) // Right paddle configuration
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
