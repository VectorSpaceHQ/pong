
#include <Arduino.h>
#include <Servo.h>

#include "Configs.h"
#include "LaserCtrl.h"
#include "Shape.h"

// TODO: If ymax, xmin, etc, never change, than use #defines instead of variables
#define  SERVO_MIN_X    (1000)
#define  SERVO_MAX_X    (2000)
#define  SERVO_MIN_Y    (1000)
#define  SERVO_MAX_Y    (2000)
#define  SERVO_MID_X    ((SERVO_MAX_X + SERVO_MIN_X) / 2)
#define  SERVO_MID_Y    ((SERVO_MAX_Y + SERVO_MIN_Y) / 2)


LaserCtrl::LaserCtrl(LaserConf& conf):
   x(SERVO_MID_X),
   y(SERVO_MID_Y),
   hskew(0),
   vskew(0),
   laserOn(false),
   shape(),
   currentVertex(0)
{
   xServo.attach(conf.xPin);
   yServo.attach(conf.yPin);
   laserPin = conf.laserPin;

   pinMode(laserPin, OUTPUT);
}


void LaserCtrl::SetShape(const Shape& _shape, uint32_t scale)
{
   // Copy the shape
   shape = _shape;
   shape.Scale(scale);     // Scale the shape
   shape.Add(x, y);        // Center the shape
}


void LaserCtrl::SetPosition(uint32_t atX, uint32_t atY)
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

