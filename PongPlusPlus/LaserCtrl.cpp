
#include <Arduino.h>
#include <Servo.h>
#include <stdlib.h>

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
   stepRateUs(US_PER_STEP),
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


void LaserCtrl::UpdateShape(uint32_t scale, bool restart, bool needToCopy, uint32_t newInterval)
{
   // Lasers work on the View set of coordinates
   if(needToCopy)
   {
      shape.CopyVerticesToView();
   }

   // Set the draw speed
   stepRateUs = newInterval;

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


void LaserCtrl::SetPosition(CoordType atX, CoordType atY)
{
   // Update our X/Y coordinates mapping from engine coordinates to servo values
   // TODO:: Need a percentage scale

   // Shift to Laser coordinates

   CoordType newX = 3000 - (SERVO_MID_X + atX + (cal.xOffset * cal.xOrientation));
   CoordType newY = SERVO_MID_Y + atY + (cal.yOffset * cal.yOrientation);


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
  Serial.print("Setting limits for ");
  Serial.print(name);
  Serial.print(" @ ");
  Serial.print(xMin);
  Serial.print(", ");
  Serial.print(xMax);
  Serial.print(", ");
  Serial.print(yMin);
  Serial.print(", ");
  Serial.print(yMax);
  Serial.print(",   ");
  
   xMin = 3000 - (SERVO_MID_X + xMin + (cal.xOffset * cal.xOrientation));
   xMax = 3000 - (SERVO_MID_X + xMax + (cal.xOffset * cal.xOrientation));
   yMin = SERVO_MID_Y + yMin + (cal.yOffset * cal.yOrientation);
   yMax = SERVO_MID_Y + yMax + (cal.yOffset * cal.yOrientation);

   // servo x-axis is reversed 
   displayMin.x = xMax;
   displayMax.x = xMin;
   displayMin.y = yMin;
   displayMax.y = yMax;


   // bounds check against physical servo limit
   if(displayMin.x < SERVO_MIN_X) { displayMin.x = SERVO_MIN_X; }
   if(displayMin.x > SERVO_MAX_X) { displayMin.x = SERVO_MAX_X; }
   if(displayMax.x < SERVO_MIN_X) { displayMax.x = SERVO_MIN_X; }
   if(displayMax.x > SERVO_MAX_X) { displayMax.x = SERVO_MAX_X; }
   if(displayMin.y < SERVO_MIN_Y) { displayMin.y = SERVO_MIN_Y; }
   if(displayMin.y > SERVO_MAX_Y) { displayMin.y = SERVO_MAX_Y; }
   if(displayMax.y < SERVO_MIN_Y) { displayMax.y = SERVO_MIN_Y; }
   if(displayMax.y > SERVO_MAX_Y) { displayMax.y = SERVO_MAX_Y; }

   
   Serial.print(displayMin.x);
   Serial.print(", ");
   Serial.print(displayMax.x);
   Serial.print(", ");
   Serial.print(displayMin.y);
   Serial.print(", ");
   Serial.println(displayMax.y);
   
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
      interval = (waitX * stepRateUs);
   }
   else
   {
      interval = (waitY * stepRateUs);
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

   // Check for out of bounds
   if(currentPosition.x < displayMin.x) { currentPosition.x = displayMin.x; }
   if(currentPosition.x > displayMax.x) { currentPosition.x = displayMax.x; }
   if(currentPosition.y < displayMin.y) { currentPosition.y = displayMin.y; }
   if(currentPosition.y > displayMax.y) { currentPosition.y = displayMax.y; }
   
   xServo.writeMicroseconds(currentPosition.x);
   yServo.writeMicroseconds(currentPosition.y);

   // Serial.print("Writing servo microseconds, ");
   // Serial.print(currentPosition.x);
   // Serial.print(", ");
   // Serial.println(currentPosition.y);
   // Serial.print(", ");
   // Serial.print(displayMin.x);
   // Serial.print(", ");
   // Serial.print(displayMax.x);
   // Serial.print(", ");
   // Serial.print(displayMin.y);
   // Serial.print(", ");
   // Serial.println(displayMax.y);
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

   if (diffX > 0)
   {
      step.x = max(step.x, 1);
   }
   else if (diffX < 0)
   {
      step.x = min(step.x, -1);
   }

   if (diffY > 0)
   {
      step.y = max(step.y, 1);
   }
   else if (diffY < 0)
   {
      step.y = min(step.y, -1);
   }

   // Serial.print("Moving steps, ");
   // Serial.print(step.x);
   // Serial.print(", ");
   // Serial.println(step.y);

   step.draw = destination.draw;

   SetWaitTime(step.x, step.y);
   SetLaser(destination.draw);
}
