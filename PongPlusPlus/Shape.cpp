/*
 * Shape.cpp
 *
 *  Created on: Aug 24, 2018
 *      Author: athiessen
 */


#include <Arduino.h>
//#include <BasicLinearAlgebra.h>

#include "Shape.h"


void Vertex::Log()
{
   Serial.print("Vertex(");
   Serial.print((int)x);
   Serial.print(", ");
   Serial.print((int)y);
   Serial.print(", ");
   Serial.print((bool)draw);
   Serial.println(")");
}


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


Shape::Shape(const Shape& other)
{
   if(this != &other)
   {
      scale = other.scale;
      CopyVertices(other.numVertices, other.vertices);
   }
}


void Shape::Scale(uint32_t _scale)
{
   scale = _scale;

   for(uint32_t cntr = 0; cntr < numVertices; cntr++)
   {
      vertices[cntr].x *= scale;
      vertices[cntr].y *= scale;
   }
}


void Shape::Add(CoordType x, CoordType y)
{
   for(uint32_t cntr = 0; cntr < numVertices; cntr++)
   {
      vertices[cntr].x += x;
      vertices[cntr].y += y;
   }
}


void Shape::Log()
{
   for(uint32_t cntr = 0; cntr < numVertices; cntr++)
   {
      vertices[cntr].Log();
   }
}

void Shape::AddVertex(CoordType x, CoordType y, bool draw)
{
   if(numVertices < MAX_VERTICES)
   {
      vertices[numVertices].x = x;
      vertices[numVertices].y = y;
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
