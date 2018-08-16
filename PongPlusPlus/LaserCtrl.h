
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
    LaserCtrl(ShapeType _shape, LaserConf& conf);

    /***
     * Calibrate the display in order to set an x min and max, and
     * a y min and max.
     */
    void Calibrate(void);

    /***
     * Control the laser and the gimble to draw the shape
     * @param atX - The X position at which to draw the shape
     * @param atY - The Y position at which to draw the shape
     */
    void Draw(uint32_t atX, uint32_t atY);

    /***
     * Change the shape of this laser to a rectangle
     * @param _width - The width of the rectangle
     * @param _height - The height of the rectangle
     */
    void SetRectangle(uint32_t _width, uint32_t _height);

    /***
     * Change the shape of this laser to a circle
     * @param _radius - The radius of the circle
     */
    void SetCircle(uint32_t _radius);

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
    ShapeType shape;
    uint32_t  x;
    uint32_t  y;
    uint32_t  xmin;
    uint32_t  ymin;
    uint32_t  xmax;
    uint32_t  ymax;
    uint32_t  width;
    uint32_t  height;
    uint32_t  radius;
    bool      laserOn;
    Servo     xServo;
    Servo     yServo;
    uint8_t   laserPin;

    void DrawRectangle(void);
    void DrawCircle(void);
    void SetLaser(void);
};

#endif    // __laser_ctrl_h__


