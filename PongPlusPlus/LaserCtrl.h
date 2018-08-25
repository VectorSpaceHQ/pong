
#ifndef __laser_ctrl_h__
#define __laser_ctrl_h__

#include <Servo.h>
#include <stdint.h>

#include "Configs.h"
#include "Shapes.h"


class LaserCtrl
{
public:
   /***
    * Constructor
    * @param _shape - The type of the shape
    * @param conf - The configuration for the laser
    * @param laserPin - The pin number for the laser control
    */
   LaserCtrl(LaserConf& conf);

    /***
    * Control the laser and the gimble to draw the shape
    * @param atX - The X position at which to draw the shape
    * @param atY - The Y position at which to draw the shape
    */
   void Draw(uint32_t atX, uint32_t atY);

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
   uint32_t    x;
   uint32_t    y;
   uint32_t    hskew;
   uint32_t    vskew;
   bool        laserOn;
   Servo       xServo;
   Servo       yServo;
   uint8_t     laserPin;
   uint32_t    currentVertex;

   /*
   void DrawRectangle(void);
   void DrawCircle(void);
   */
   void SetLaser(void);
};

#endif    // __laser_ctrl_h__


