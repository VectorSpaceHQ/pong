
#include <Arduino.h>
#include <Servo.h>

#include "Configs.h"
#include "LaserCtrl.h"
#include "Shapes.h"
#include "Controller.h"

// TODO: If ymax, xmin, etc, never change, than use #defines instead of variables
#define  SERVO_MIN_X    (544)
#define  SERVO_MAX_X    (2400)
#define  SERVO_MIN_Y    (544)
#define  SERVO_MAX_Y    (2400)

// Define these for now.
// TODO: Find a way to pass these in
#define  GAME_WIDTH     (800)
#define  GAME_HEIGHT    (600)

LaserCtrl::LaserCtrl(ShapeType _shape, LaserConf& conf):
   shape(_shape),
   x(0),
   y(0),
   hskew(0),
   vskew(0),
   xmin(544),
   xmax(2400),
   ymin(544),
   ymax(2400),
   width(0),
   height(0),
   radius(0),
   laserOn(false)
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
   x = xmin + atX * (xmax - xmin) / GAME_WIDTH;
   y = ymin + atY * (ymax - ymin) / GAME_HEIGHT;

   switch(shape)
   {
      case ShapeRectangle:
         DrawRectangle();
         break;

      case ShapeCircle:
         DrawCircle();
         break;

      default:
         break;
   }
}


void LaserCtrl::SetBounds(uint32_t _xmin, uint32_t _xmax, uint32_t _ymin,
                          uint32_t _ymax, uint32_t _hskew, uint32_t _vskew)
{
   xmin = _xmin;
   xmax = _xmax;
   ymin = _ymin;
   ymax = _ymax;
   hskew = _hskew;
   vskew = _vskew;
}


void LaserCtrl::SetRectangle(uint32_t _width, uint32_t _height)
{
   shape = ShapeRectangle;
   width = _width;
   height = _height;
}


void LaserCtrl::SetCircle(uint32_t _radius)
{
   shape = ShapeRectangle;
   radius = _radius;
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


void LaserCtrl::SetLaser()
{
   digitalWrite(laserPin, (laserOn ? HIGH : LOW));
}

