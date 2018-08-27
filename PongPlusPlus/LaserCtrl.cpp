
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

#define  US_PER_STEP    (1000)

static uint32_t absolute(int32_t val);

LaserCtrl::LaserCtrl(LaserConf& conf, Shape& _shape, const char* _name):
   TimedInterval(400),     // This will change based on laser movement
   name(_name),
   x(SERVO_MID_X),
   y(SERVO_MID_Y),
   hskew(0),
   vskew(0),
   laserOn(false),
   shape(_shape),
   currentVertex(0),
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


void LaserCtrl::UpdateShape(uint32_t scale)
{
   //shape.Scale(-1);      // Invert the shape
   shape.Scale(scale);     // Scale the shape
   shape.Add(x, y);        // Center the shape

   Serial.print("New Shape for ");
   Serial.println(name);
   shape.Log();

   // Reset the shape
   currentVertex = 0;
   Move(shape.vertices[currentVertex]);
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


void LaserCtrl::SetWaitTime(int32_t x, int32_t y)
{
   // TODO: Need to handle wrap (every 70 minutes)

   uint32_t waitX = absolute(x);
   uint32_t waitY = absolute(y);

//   Serial.print("x,y: ");
//   Serial.print(waitX);
//   Serial.print(", ");
//   Serial.println(waitY);

   if(waitX >= waitY)
   {
      interval = (waitX * US_PER_STEP);
   }
   else
   {
      interval = (waitY * US_PER_STEP);
   }

//   Serial.println(interval);
}


void LaserCtrl::Update()
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

   if(abs(destination.x - currentPosition.x) < abs(step.x))
   {
      stepX = (destination.x - currentPosition.x);
   }

   if(abs(destination.y - currentPosition.y) < abs(step.y))
   {
      stepY = (destination.y - currentPosition.y);
   }

   SetWaitTime(stepX, stepY);

   currentPosition.x += stepX;
   currentPosition.y += stepY;

   xServo.writeMicroseconds(currentPosition.x);
   yServo.writeMicroseconds(currentPosition.y);
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

   step.x = diffX / shape.scale;
   step.y = diffY / shape.scale;
   step.draw = destination.draw;

   SetWaitTime(step.x, step.y);
   SetLaser(destination.draw);
}


uint32_t absolute(int32_t val)
{
   return static_cast<uint32_t>(static_cast<uint16_t>(~abs(val) + 1));
}
