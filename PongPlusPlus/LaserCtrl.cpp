
#include <Arduino.h>
#include <Servo.h>

#include "Configs.h"
#include "LaserCtrl.h"
#include "Shapes.h"

// TODO: If ymax, xmin, etc, never change, than use #defines instead of variables
#define  SERVO_MIN_X    (1000)
#define  SERVO_MAX_X    (2000)
#define  SERVO_MIN_Y    (1000)
#define  SERVO_MAX_Y    (2000)

// Define these for now.
// TODO: Find a way to pass these in
#define  GAME_WIDTH     (800)
#define  GAME_HEIGHT    (600)

LaserCtrl::LaserCtrl(LaserConf& conf):
   x(0),
   y(0),
   hskew(0),
   vskew(0),
   laserOn(false),
   currentVertex(0)
{
   xServo.attach(conf.xPin);
   yServo.attach(conf.yPin);
   laserPin = conf.laserPin;

   pinMode(laserPin, OUTPUT);
}


void LaserCtrl::Draw(uint32_t atX, uint32_t atY)
{
   // Update our X/Y coordinates mapping from engine coordinates to servo values
   // TODO:: Need game_width and game_height passed from the engine
   x = SERVO_MIN_X + atX * (SERVO_MAX_X - SERVO_MIN_X) / GAME_WIDTH;
   y = SERVO_MIN_Y + atY * (SERVO_MAX_Y - SERVO_MIN_Y) / GAME_HEIGHT;
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

