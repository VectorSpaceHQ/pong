
#ifndef __laser_ctrl_h__
#define __laser_ctrl_h__

#include <Servo.h>
#include <stdint.h>

#include "Configs.h"
#include "TimedInterval.h"
#include "Shape.h"


class LaserCtrl: public TimedInterval
{
public:
   /***
    * Constructor
    */
   LaserCtrl(LaserConf& conf, Shape& _shape, const char* _name);

   /***
    * This method is called afer the shape given to the laser changes
    */
   void UpdateShape(uint32_t scale);

    /***
    * Control the laser and the gimble to draw the shape
    * @param atX - The X position at which to draw the shape
    * @param atY - The Y position at which to draw the shape
    */
   void SetPosition(CoordType atX, CoordType atY);

   /***
    * Sets the wait time to
    */
   void SetWaitTime(int32_t x, int32_t y);

   /***
    * Draw the specified score at the given coordinates
    */
   void DrawScore(uint32_t atX, uint32_t atY, uint16_t score);

   /***
    * Turn the laser on
    */
   void On(void);

   /***
    * Turn the laser off
    */
   void Off(void);

   /***
    * Toggle the laser
    */
   void Toggle(void);
   void Update();

private:
   const char*       name;
   uint32_t    x;
   uint32_t    y;
   uint32_t    hskew;
   uint32_t    vskew;
   bool        laserOn;
   Servo       xServo;
   Servo       yServo;
   uint8_t     laserPin;
   Shape&      shape;
   uint32_t    currentVertex;
   unsigned long    waitTime;

   Vertex      currentPosition;
   Vertex      destination;
   Vertex      step;

   /*
   void DrawRectangle(void);
   void DrawCircle(void);
   */
   void SetLaser(void);
   void SetLaser(bool onOff);

   /***
    * Command the laser to move to a new vertex
    */
   void Move(Vertex& dest);
};

#endif    // __laser_ctrl_h__


