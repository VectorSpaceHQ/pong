
#ifndef __shapes_h__
#define __shapes_h__

#include <stdint.h>


enum ShapeType
{
  ShapeRectangle  = 0,
  ShapeCircle
};


struct Rectangle
{
  public:
    uint32_t  x;
    uint32_t  y;
    uint32_t  width;
    uint32_t  height;

    Rectangle(uint32_t _x, uint32_t _y, uint32_t _w, uint32_t _h):
      x(_x),
      y(_y),
      width(_w),
      height(_h)
    {
    }
};


struct Circle
{
  public:
    uint32_t  x;
    uint32_t  y;
    uint32_t  radius;

    Circle(uint32_t _x, uint32_t _y, uint32_t _rad):
      x(_x),
      y(_y),
      radius(_rad)
    {
    }
};

#endif  // __shapes_h__

