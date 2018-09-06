
#include <Arduino.h>
#include <Servo.h>
#include <stdlib.h>
#include <MatrixMath.h>

#include "Configs.h"
#include "LaserCtrl.h"
#include "Model.h"
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


LaserCtrl::LaserCtrl(LaserConf&                conf,
                     Model::LaserCalibration&  _cal,
                     Shape&                    _shape,
                     const char*               _name):
   TimedInterval(400),     // This will change based on laser movement
   name(_name),
   cal(_cal),
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

   displayMin.x = 1000;
   displayMax.x = 2000;
   displayMin.y = 1000;
   displayMax.y = 2000;

   // Turn off and center the laser
   currentPosition.x = x;
   currentPosition.y = y;
   currentPosition.draw = laserOn;
   SetLaser(currentPosition.draw);
   xServo.writeMicroseconds(x);
   yServo.writeMicroseconds(y);
}


void LaserCtrl::UpdateShape(uint32_t scale, bool restart, bool needToCopy)
{
   // Lasers work on the View set of coordinates

   if(needToCopy)
   {
      shape.CopyVerticesToView();
   }

   shape.SetOrientation(CoordsView, cal.xOrientation, cal.yOrientation);   // Invert the shape if necessary
   shape.Scale(CoordsView, scale);     // Scale the shape
   shape.Add(CoordsView, x, y);        // Center the shape

  // Serial.print("New Shape for ");
  // Serial.println(name);
  // Serial.print(" @ x,y = ");
  // Serial.print(x);
  // Serial.print(", ");
  // Serial.println(y);
  
  // shape.Log();
  // delay(200);

   // Reset the shape
   if(restart)
   {
      currentVertex = 0;
      Move(shape.viewVertices[currentVertex]);
   }
}


// void LaserCtrl::translateGameToServoCoords(CoordType x, CoordType y)
// {
//   // Go from game x,y coords to servo u,v coords
//   // by apply homography matrix

//   a = matrix(x,
// 	     y,
// 	     1);

//   solution = dot(H, a);
// }


void LaserCtrl::SetPosition(CoordType atX, CoordType atY)
{
   // Update our X/Y coordinates mapping from engine coordinates to servo values
   // TODO:: Need a percentage scale

   // Shift to Laser coordinates

   CoordType newX = 3000 - (SERVO_MID_X + atX + (cal.xOffset * cal.xOrientation));
   CoordType newY = SERVO_MID_Y + atY + (cal.yOffset * cal.yOrientation);

/*
   Serial.print(name);
   Serial.print(" Position( ");
   Serial.print(cal.xOffset);
   Serial.print(", ");
   Serial.print(cal.yOffset);
   Serial.print(", ");
   Serial.print(newX);
   Serial.print(", ");
   Serial.print(newY);
   Serial.println(")");
*/

   // Move our vertex coordinates the difference
   if(running)
   {
      shape.Add(CoordsView, (newX - x), (newY - y));
   }

   // Update our new coordinates with the new ones
   x = newX;
   y = newY;
}


void LaserCtrl::Move(CoordType atX, CoordType atY)
{
   SetPosition(atX, atY);

   xServo.writeMicroseconds(x);
   yServo.writeMicroseconds(y);
}


void LaserCtrl::SetLimits(CoordType xMin, CoordType yMin, CoordType xMax, CoordType yMax)
{
   displayMin.x = xMin;
   displayMin.y = yMin;
   displayMax.x = xMax;
   displayMax.y = yMax;
}


void LaserCtrl::SetWaitTime(int32_t x, int32_t y)
{
   // TODO: Need to handle wrap (every 70 minutes)

   uint32_t waitX = abs(x);
   uint32_t waitY = abs(y);

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
      Move(shape.viewVertices[currentVertex]);
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

   // Check for out of bounds
   if(destination.x < displayMin.x) { destination.x = displayMin.x; }
   if(destination.x > displayMax.x) { destination.x = displayMax.x; }
   if(destination.y < displayMin.y) { destination.y = displayMin.y; }
   if(destination.y > displayMax.y) { destination.y = displayMax.y; }

   CoordType diffX = (destination.x - currentPosition.x);
   CoordType diffY = (destination.y - currentPosition.y);

   step.x = diffX / shape.scale;
   step.y = diffY / shape.scale;
   step.draw = destination.draw;

   SetWaitTime(step.x, step.y);
   SetLaser(destination.draw);
}

void LaserCtrl::ApplyHomography(int32_t x, int32_t y)
{
  mtx_type a[3][1];
  a[0][0] = x;
  a[1][0] = y;
  a[2][0] = 1;

  mtx_type solution[3][1];
  Matrix.Multiply((mtx_type*)cal.H, (mtx_type*)a, 3, 3, 1, (mtx_type*)solution);
  Matrix.Print((mtx_type*)solution, 3, 1, "solution");

  int u = solution[0][0];
  int v = solution[1][0];

  Serial.print("Homography from x,y: ");
  Serial.print(x);
  Serial.print(", ");
  Serial.print(y);
  Serial.print(" to: ");
  Serial.print(u);
  Serial.print(", ");
  Serial.println(v);
}
