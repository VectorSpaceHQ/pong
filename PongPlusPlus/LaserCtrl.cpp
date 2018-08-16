
#include <Arduino.h>
#include <Servo.h>

#include "Configs.h"
#include "LaserCtrl.h"
#include "Shapes.h"


LaserCtrl::LaserCtrl(ShapeType _shape, LaserConf& conf):
  shape(_shape),
  x(0),
  y(0),
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


void LaserCtrl::Calibrate()
{
  xmin=544;
  ymin=544;
  xmax=2400;
  ymax=2400;
}


void LaserCtrl::Draw(uint32_t atX, uint32_t atY)
{
  // Update our X/Y coordinates mapping from engine coordinates to servo values
  x = xmin + (atX-engine_xmin) * (xmax - xmin) / (engine_xmax - engine_xmin);
  y = ymin + (atY-engine_ymin) * (ymax - ymin) / (engine_ymax - engine_ymin);

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
  // x,y marks center of rectangle
  for(int j = y-height/2; j <= y+height/2; j++){
    yServo.writeMicroseconds(j);
    delayMicroseconds(5);
  }
  delay(60);
  for(int i = x-width/2; i <= x+width/2; i++){
    xServo.writeMicroseconds(i);
    delayMicroseconds(5);
  }
  delay(60);
  for(int j = y+height/2-1; j >= y-height/2; j--){
    yServo.writeMicroseconds(j);
    delayMicroseconds(5);
  }
  delay(60);
  for(int i = x+width/2-1; i >= x-width/2; i--){
    xServo.writeMicroseconds(i);
    delayMicroseconds(5);
  }
}


void LaserCtrl::DrawCircle(void)
{
  // x,y marks center of circle
  // i,j marks edge of circle
  for(int i=x-r; i<x+r; i++){
    j = y + sqrt(r^2 - (i-x)^2);
    xServo.writeMicroseconds(i);
    yServo.writeMicroseconds(j);
    delayMicroseconds(5);
  }
  for(int i=x+r; i<x-r; i--){
    j = y - sqrt(r^2 - (i-x)^2);
    xServo.writeMicroseconds(i);
    yServo.writeMicroseconds(j);
    delayMicroseconds(5);
  }
}


void LaserCtrl::SetLaser()
{
  digitalWrite(laserPin, (laserOn ? HIGH : LOW));
}

