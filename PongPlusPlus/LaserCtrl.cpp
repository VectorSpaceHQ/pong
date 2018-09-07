
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
   oldX(SERVO_MID_X),
   oldY(SERVO_MID_Y),
   xPin(conf.xPin),
   yPin(conf.yPin),
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

   
   if(name == "Ball"){
     Serial.print("New Shape for ");
     Serial.println(name);
     Serial.print(" @ x,y = ");
     Serial.print(x);
     Serial.print(", ");
     Serial.println(y);
   }
  
  // shape.Log();
  // delay(200);

   // Reset the shape
   if(restart)
   {
     // reset center position
     Serial.print("Resetting ");
     Serial.println(name);
     delay(100);
     currentPosition.x = SERVO_MID_X;
     currentPosition.y = SERVO_MID_Y;
     destination.x = SERVO_MID_X;
     destination.y = SERVO_MID_Y;
     // ------- End Debug ------
     
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


   
   if (x != oldX){
     xServo.writeMicroseconds(x);
     oldX = x;
   }
   if (y != oldY){
     yServo.writeMicroseconds(y);
     oldY = y;
   }
   
   
}


void LaserCtrl::SetWaitTime(int32_t x, int32_t y)
{
   // TODO: Need to handle wrap (every 70 minutes)

   uint32_t waitX = abs(x);
   uint32_t waitY = abs(y);

   if(waitX >= waitY)
   {
      interval = (waitX * US_PER_STEP);
   }
   else
   {
      interval = (waitY * US_PER_STEP);
   }

}


void LaserCtrl::Update()
{

  // if(name == "Ball"){
  //   Serial.print("Ball updating position, ");
  //   Serial.print(currentPosition.x);
  //   Serial.print(", ");
  //   Serial.print(destination.x);
  //   Serial.print(", ");
  //   Serial.print(currentPosition.y);
  //   Serial.print(", ");
  //   Serial.println(destination.y);
  // }

  
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

   if (currentPosition.x != oldX){
     xServo.writeMicroseconds(currentPosition.x);
     oldX = x;
   }
   if (currentPosition.y != oldY){
     yServo.writeMicroseconds(currentPosition.y);
     oldY = currentPosition.y;

     // if(millis()%10==0){
     //   Serial.print("writing to servo, ");
     //   Serial.print(name);
     //   Serial.print(", ");
     //   Serial.println(currentPosition.y);
     // }
     
   }
   

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

   // if(name == "Ball"){
   //   Serial.print("Ball Moving position, ");
   //   Serial.print(currentPosition.x);
   //   Serial.print(", ");
   //   Serial.print(destination.x);
   //   Serial.print(", ");
   //   Serial.print(currentPosition.y);
   //   Serial.print(", ");
   //   Serial.println(destination.y);
   // }

   // if diff is nonzero, step must be nonzero
   // previous bug had diff of 1 but step of 0, so never reached destination

   // Why divide by the scale? Aren't we moving the center point?
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

   step.draw = destination.draw;


   // if(name == "LeftPaddle"){
   //   Serial.print("LaserCTRL::MOVE step.y, ");
   //   Serial.print(step.y);
   //   Serial.print(", ");
   //   Serial.print(shape.scale);
   //   Serial.print(", ");
   //   Serial.println(diffY);
   // }

   SetWaitTime(step.x, step.y);
   SetLaser(destination.draw);
}
