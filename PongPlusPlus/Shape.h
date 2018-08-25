/*
 * Shape.h
 *
 *  Created on: Aug 24, 2018
 *      Author: athiessen
 */

#ifndef __shape_h__
#define __shape_h__

//#include <BasicLinearAlgebra.h>
#include <stdint.h>


#define MAX_VERTICES             (16)

typedef int32_t CoordType;

//typedef BLA::Matrix<2,16,BLA::Array<2,16,int> > VertexMatrix;

struct Vertex
{
public:
   CoordType   x;
   CoordType   y;
   bool        draw;

   void Log();
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
   void Add(CoordType x, CoordType y);
   void Log();

private:

   void CopyVertices(uint32_t _numVertices, const Vertex* _vertices);
};


#endif   // __shape_h__
