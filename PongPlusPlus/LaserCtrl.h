
#ifndef __laser_ctrl_h__
#define __laser_ctrl_h__

#include <Servo.h>
#include <stdint.h>

#include "Configs.h"
#include "Model.h"
#include "Shape.h"
#include "TimedInterval.h"

#define  US_PER_STEP    (1000)


class LaserCtrl: public TimedInterval
{
public:
   /***
    * Constructor
    */
   LaserCtrl(LaserConf&                conf,
             Model::LaserCalibration&  _cal,
             Shape&                    _shape,
             const char*               _name);

   /***
    * This method is called afer the shape given to the laser changes
    */
   void UpdateShape(uint32_t scale, bool restart, bool needToCopy, uint32_t newInterval = US_PER_STEP);

    /***
    * Control the laser and the gimble to draw the shape
    * @param atX - The X position at which to draw the shape
    * @param atY - The Y position at which to draw the shape
    */
   void SetPosition(CoordType atX, CoordType atY);

   void Move(CoordType atX, CoordType atY);

   void SetLimits(CoordType xMin, CoordType yMin, CoordType xMax, CoordType yMax);

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

   /***
    * This method is called periodically, based on the TimedInterval schedule time
    */
   void Update();

private:
   const char*                name;
   Model::LaserCalibration&   cal;
   uint32_t                   x;
   uint32_t                   y;
   uint32_t                   hskew;
   uint32_t                   vskew;
   bool                       laserOn;
   uint32_t                   stepRateUs;
   Servo                      xServo;
   Servo                      yServo;
   uint8_t                    laserPin;
   Shape&                     shape;
   uint32_t                   currentVertex;    // Which vertex in the shape is the laser drawing to?

   Vertex                     displayMin;       // Bottom left vertex
   Vertex                     displayMax;       // Top right vertex

   Vertex                     currentPosition;  // Where the laser currently is pointed while stepping
   Vertex                     destination;      // Where the laser is going in the current sweep
   Vertex                     step;             // The slope of the line/sweep the laser is making

   void SetLaser(void);
   void SetLaser(bool onOff);

   /***
    * Command the laser to move to a new vertex
    */
   void Move(Vertex& dest);

   /***
    * Sets the wait time according to the largest step
    */
   void SetWaitTime(int32_t x, int32_t y);
};

#endif    // __laser_ctrl_h__


