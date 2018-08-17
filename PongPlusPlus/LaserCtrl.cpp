
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
  // x,y marks center of rectangle
  for(int j = y-height/2; j <= y+height/2; j++){
    yServo.write(j);
    delay(2);
  }  
  for(int i = x-width/2; i <= x+width/2; i++){
    xServo.write(i);
    delay(2);
  }
  for(int j = y+height/2; j >= y-height/2; j--){
    yServo.write(j);
    delay(2);
  }  
  for(int i = x+width/2; i >= x-width/2; i--){
    xServo.write(i);
    delay(2);
  }
}


void LaserCtrl::DrawCircle(void)
{
  // x,y marks center of circle
  // a,b marks edge of circle
  int b;
  for(int a=x-radius; a<x+radius; a++){
    b = y + sqrt(radius^2 - (a-x)^2);
    xServo.write(a);
    yServo.write(b);
    delay(1);
  }
  for(int a=x+radius; a<x-radius; a--){
    b = y - sqrt(radius^2 - (a-x)^2);
    xServo.write(a);
    yServo.write(b);
    delay(1);
  }
}


void LaserCtrl::SetLaser()
{
  digitalWrite(laserPin, (laserOn ? HIGH : LOW));
}

