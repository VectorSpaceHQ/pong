
#include <Arduino.h>
#include <Servo.h>

#include "Configs.h"
#include "LaserCtrl.h"
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
   shape.Scale(scale);     // Scale the shape
   shape.Add(x, y);        // Center the shape

   Serial.print("Laser '");
   Serial.print(name);
   Serial.print("' New Shape: ");
   shape.Log();

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


void LaserCtrl::Step()
{
   waitTime -= MAIN_LOOP_TIME;

   if(waitTime <= 0)
   {
      // Have we reached our destination?
      if( (currentPosition.x == destination.x) && (currentPosition.y == destination.y))
      {
         if(++currentVertex > shape.numVertices)
         {
            currentVertex = 0;
         }

         // Get our next destination
         Move(shape.vertices[currentVertex]);
      }

      CoordType   stepX = step.x;
      CoordType   stepY = step.y;

#warning Probably need to do something with ABS here
      if((destination.x - currentPosition.x) < step.x)
      {
         stepX = (destination.x - currentPosition.x);
      }

      if((destination.y - currentPosition.y) < step.y)
      {
         stepY = (destination.y - currentPosition.y);
      }

      if(stepX >= stepY)
      {
         waitTime = (stepX * US_PER_STEP);
      }
      else
      {
         waitTime = (stepY * US_PER_STEP);
      }

      currentPosition.x += stepX;
      currentPosition.y += stepY;

      xServo.writeMicroseconds(currentPosition.x);
      yServo.writeMicroseconds(currentPosition.y);

      Serial.print("Laser '");
      Serial.print(name);
      Serial.print("' Current Pos ");
      currentPosition.Log();
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

/*
void LaserCtrl::DrawRectangle(void)
{
   // x,y marks center of rectangle
   for(int j = y - height / 2; j <= y + height / 2; j++)
   {
      yServo.writeMicroseconds(j);
      delayMicroseconds(5);
   }
   delay(60);
   for(int i = x - width / 2; i <= x + width / 2; i++)
   {
      xServo.writeMicroseconds(i);
      delayMicroseconds(5);
   }
   delay(60);
   for(int j = y + height / 2 - 1; j >= y - height / 2; j--)
   {
      yServo.writeMicroseconds(j);
      delayMicroseconds(5);
   }
   delay(60);
   for(int i = x + width / 2 - 1; i >= x - width / 2; i--)
   {
      xServo.writeMicroseconds(i);
      delayMicroseconds(5);
   }
}
*/

/*
void LaserCtrl::DrawCircle(void)
{
   // x,y marks center of circle
   // i,j marks edge of circle
   for(int i = x - radius; i < (x + radius); i++)
   {
      int j = y + sqrt((radius ^ 2) - ((i - x) ^ 2));
      xServo.writeMicroseconds(i);
      yServo.writeMicroseconds(j);
      delayMicroseconds(5);
   }
   for(int i = x + radius; i < x - radius; i--)
   {
      int j = y - sqrt((radius ^ 2) - ((i - x) ^ 2));
      xServo.writeMicroseconds(i);
      yServo.writeMicroseconds(j);
      delayMicroseconds(5);
   }
}
*/


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

   if(step.x >= step.y)
   {
      waitTime = (step.x * US_PER_STEP);
   }
   else
   {
      waitTime = (step.y * US_PER_STEP);
   }

   SetLaser(destination.draw);

   Serial.print("Laser '");
   Serial.print(name);
   Serial.print("' New Destination ");
   destination.Log();
}
