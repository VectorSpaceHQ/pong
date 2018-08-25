
#include <Arduino.h>
#include <Servo.h>
#include <stdlib.h>

#include "Configs.h"
#include "LaserCtrl.h"
#include "ScheduledInterval.h"
#include "Shape.h"
#include "Timing.h"

// TODO: If ymax, xmin, etc, never change, than use #defines instead of variables
#define  SERVO_MIN_X    (1000)
#define  SERVO_MAX_X    (2000)
#define  SERVO_MIN_Y    (1000)
#define  SERVO_MAX_Y    (2000)
#define  SERVO_MID_X    ((SERVO_MAX_X + SERVO_MIN_X) / 2)
#define  SERVO_MID_Y    ((SERVO_MAX_Y + SERVO_MIN_Y) / 2)

#define  US_PER_STEP    (200)


LaserCtrl::LaserCtrl(LaserConf& conf, const char* _name):
   ScheduledInterval(50),
   name(_name),
   x(SERVO_MID_X),
   y(SERVO_MID_Y),
   hskew(0),
   vskew(0),
   laserOn(false),
   shape(),
   currentVertex(0),
   waitTime(0),
   currentPosition(),
   destination(),
   step()
{
   // Configure the pins
   xServo.attach(conf.xPin);
   yServo.attach(conf.yPin);
   laserPin = conf.laserPin;
   pinMode(laserPin, OUTPUT);

   // Turn off and center the laser
   currentPosition.x = x;
   currentPosition.y = y;
   currentPosition.draw = laserOn;
   SetLaser(currentPosition.draw);
   xServo.writeMicroseconds(x);
   yServo.writeMicroseconds(y);
}


void LaserCtrl::SetShape(const Shape& _shape, uint32_t scale)
{
   // Copy the shape
   shape = _shape;
   //shape.Scale(-1);        // Invert the shape
   shape.Scale(scale);     // Scale the shape
   shape.Add(x, y);        // Center the shape
   ResetShape();
}


void LaserCtrl::SetPosition(CoordType atX, CoordType atY)
{
   // Update our X/Y coordinates mapping from engine coordinates to servo values
   // TODO:: Need a percentage scale

   // Shift to Laser coordinates
   CoordType newX = SERVO_MID_X + atX;
   CoordType newY = SERVO_MID_Y + atY;

   // Move our vertex coordinates the difference
   shape.Add((newX - x), (newY - y));

   // Update our new coordinates with the new ones
   x = newX;
   y = newY;
}


void LaserCtrl::ResetShape()
{
   currentVertex = 0;

   // Get our next destination
   Move(shape.vertices[currentVertex]);
}


void LaserCtrl::SetWaitTime(int32_t x, int32_t y)
{
   // Get current time in uS
   unsigned long  time = micros();

   // TODO: Need to handle wrap (every 70 minutes)

   if(x >= y)
   {
      waitTime = time + (x * US_PER_STEP);
   }
   else
   {
      waitTime = time + (y * US_PER_STEP);
   }
}


void LaserCtrl::Update()
{
   // Get current time in uS
   unsigned long  time = micros();

   if(time >= waitTime)
   {
      // Have we reached our destination?
      if( (currentPosition.x == destination.x) && (currentPosition.y == destination.y))
      {
         if(++currentVertex >= shape.numVertices)
         {
            currentVertex = 0;
         }

         // Get our next destination
         Move(shape.vertices[currentVertex]);
      }

      CoordType   stepX = step.x;
      CoordType   stepY = step.y;
/*
      if((destination.x - currentPosition.x) < step.x)
      {
         stepX = (destination.x - currentPosition.x);
      }

      if((destination.y - currentPosition.y) < step.y)
      {
         stepY = (destination.y - currentPosition.y);
      }
*/
      SetWaitTime(abs(stepX), abs(stepY));

      Serial.print("Step Wait: ");
      Serial.println(waitTime);

      currentPosition.x += stepX;
      currentPosition.y += stepY;

      xServo.writeMicroseconds(currentPosition.x);
      yServo.writeMicroseconds(currentPosition.y);
   }
}


void LaserCtrl::DrawScore(uint32_t atX, uint32_t atY, uint16_t score)
{
}


void LaserCtrl::On(void)
{
   laserOn = true;
   SetLaser();
}


void LaserCtrl::Off(void)
{
   laserOn = false;
   SetLaser();
}


void LaserCtrl::Toggle(void)
{
   laserOn = !laserOn;
   SetLaser();
}


void LaserCtrl::SetLaser()
{
   digitalWrite(laserPin, (laserOn ? HIGH : LOW));
}

void LaserCtrl::SetLaser(bool onOff)
{
   laserOn = onOff;
   SetLaser();
}


void LaserCtrl::Move(Vertex& dest)
{
   destination = dest;

   CoordType diffX = (destination.x - currentPosition.x);
   CoordType diffY = (destination.y - currentPosition.y);

   Serial.print("Scale: ");
   Serial.println(shape.scale);

   step.x = diffX / shape.scale;
   step.y = diffY / shape.scale;
   step.draw = destination.draw;

   SetWaitTime(abs(step.x), abs(step.y));
   SetLaser(destination.draw);

   Serial.print("Current: ");
   currentPosition.Log();
   Serial.print("New Destination: ");
   destination.Log();
   Serial.print("Step Size: ");
   step.Log();

   //Serial.print("Laser '");
   //Serial.print(name);
   //Serial.print("' New Destination ");
   //destination.Log();
}
