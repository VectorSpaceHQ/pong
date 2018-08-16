
#include <Arduino.h>
#include <Servo.h>

#include "Configs.h"
#include "LaserCtrl.h"
#include "Shapes.h"


LaserCtrl::LaserCtrl(ShapeType _shape, LaserConf& conf):
  shape(_shape),
  x(0),
  y(0),
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
  // Update our X/Y coordinates
  x = atX;
  y = atY;

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
  
}


void LaserCtrl::DrawCircle(void)
{
  
}


void LaserCtrl::SetLaser()
{
  digitalWrite(laserPin, (laserOn ? HIGH : LOW));
}

