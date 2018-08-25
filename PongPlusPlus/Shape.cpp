/*
 * Shape.cpp
 *
 *  Created on: Aug 24, 2018
 *      Author: athiessen
 */


//#include <BasicLinearAlgebra.h>

#include "Shape.h"


Shape::Shape():
   scale(1),
   numVertices(0)
{
   for(uint32_t cntr = 0; cntr < MAX_VERTICES; cntr++)
   {
      vertices[cntr] = { 0, 0, true };
   }
}


Shape::Shape(uint32_t _numVertices, Vertex* _vertices):
   scale(1)
{
   CopyVertices(_numVertices, vertices);
}


Shape& Shape::operator=(const Shape& other)
{
   if(this != &other)
   {
      scale = other.scale;
      CopyVertices(other.numVertices, other.vertices);
   }

   return *this;
}


void Shape::AddVertex(CoordType x, CoordType y, bool draw)
{
   if(numVertices < MAX_VERTICES)
   {
      vertices[numVertices].x = x;
      vertices[numVertices].x = y;
      vertices[numVertices].draw = draw;
      ++numVertices;
   }
}


void Shape::CopyVertices(uint32_t _numVertices, const Vertex* _vertices)
{
   numVertices = _numVertices;

   if(numVertices > MAX_VERTICES)
   {
      numVertices = MAX_VERTICES;
   }

   for(uint32_t cntr = 0; cntr < numVertices; cntr++)
   {
      vertices[cntr] = _vertices[cntr];
   }
}
