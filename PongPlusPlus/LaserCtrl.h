
#ifndef __laser_ctrl_h__
#define __laser_ctrl_h__

#include <Servo.h>
#include <stdint.h>

#include "Configs.h"
#include "Shape.h"


class LaserCtrl
{
public:
   /***
    * Constructor
    * @param _shape - The type of the shape
    * @param conf - The configuration for the laser
    * @param laserPin - The pin number for the laser control
    */
   LaserCtrl(LaserConf& conf, const char* _name);

   /***
    *
    */
   void SetShape(const Shape& _shape, uint32_t scale);

    /***
    * Control the laser and the gimble to draw the shape
    * @param atX - The X position at which to draw the shape
    * @param atY - The Y position at which to draw the shape
    */
   void SetPosition(CoordType atX, CoordType atY);
   void ResetShape();
   void Step();

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
   Shape       shape;
   uint32_t    currentVertex;
   int32_t     waitTime;

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


