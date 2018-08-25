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
   CoordType   x;
   CoordType   y;
   bool        draw;
};


class Shape
{
public:
   Shape();
   Shape(uint32_t _numVertices, Vertex* _vertices);

   void     AddVertex(CoordType _x, CoordType y, bool draw);
   Shape&   operator=(const Shape& other);

private:
   Vertex     vertices[MAX_VERTICES];
   uint32_t   numVertices;
   uint16_t   scale;

   void CopyVertices(uint32_t _numVertices, const Vertex* _vertices);
};


#endif   // __shape_h__
