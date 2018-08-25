
#include <Arduino.h>

#include "Controller.h"
#include "Engine.h"
#include "HwConfig.h"
#include "Shapes.h"
#include "Pong.h"
#include "Timing.h"
#include "View.h"


Pong::Pong():
   hardware(),                            // All the hardware settings
   display(),                             // Model display settings
   gameStatus(),                          // Model game status

   controller(hardware.leftPaddle,        // Left paddle configuration
              hardware.rightPaddle),      // Right paddle configuration

   engine(display,                        // Display settings
          gameStatus,                     // Game status
          controller.leftPaddleStatus,    // Left paddle status
          controller.rightPaddleStatus),  // Right paddle status

   view(
      display,                            // Display settings
      gameStatus,                         // Game status
      hardware.leftPlayerLaser,           // Left paddle laser configuration
      hardware.rightPlayerLaser,          // Right paddle laser configuration
      hardware.ballLaser )                // Ball laser configuration
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
   //Serial.println("----------Main----------");
   //controller.Run();    // Update paddle HW info
   //engine.Run();        // Run game engine
   view.Run();          // Draw the screen

   delayMicroseconds(MAIN_LOOP_TIME);
   //delay(MAIN_LOOP_TIME);
}
