/*
 * Shape.h
 *
 *  Created on: Aug 24, 2018
 *      Author: athiessen
 */

#ifndef __shape_h__
#define __shape_h__

#include <stdint.h>


#define MAX_VERTICES             (16)

typedef int32_t CoordType;


struct Vertex
{
public:
   CoordType   x;
   CoordType   y;
   bool        draw;

   void Log();
};

enum ShapeType
{
   ShapeTypeNumeral0 = 0,
   ShapeTypeNumeral1 = 1,
   ShapeTypeNumeral2 = 2,
   ShapeTypeNumeral3 = 3,
   ShapeTypeNumeral4 = 4,
   ShapeTypeNumeral5 = 5,
   ShapeTypeNumeral6 = 6,
   ShapeTypeNumeral7 = 7,
   ShapeTypeNumeral8 = 8,
   ShapeTypeNumeral9 = 9,

   ShapeTypeCenterViewCal,

   ShapeTypePaddle,
   ShapeTypeBall
};


class Shape
{
public:
   Shape();
   Shape(uint32_t _numVertices, Vertex* _vertices);
   Shape(const Shape& other);

   uint16_t   scale;
   uint32_t   numVertices;
   Vertex     vertices[MAX_VERTICES];

   void AddVertex(CoordType x, CoordType y, bool draw);
   void Scale(CoordType _scale);
   void SetOrientation(CoordType _scaleX, CoordType _scaleY);
   void Add(CoordType x, CoordType y);
   void CreateShape(int numeral);
   void Reset();
   void Log();

private:

   void CopyVertices(uint32_t _numVertices, const Vertex* _vertices);
};


#endif   // __shape_h__
