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
   numVertices(0),
   position(),
   vector(),
   acceleration(0)
{
   for(uint32_t cntr = 0; cntr < MAX_VERTICES; cntr++)
   {
      vertices[cntr] = { 0, 0, false };
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


void Shape::Scale(CoordType _scale)
{
   scale = _scale;

   for(uint32_t cntr = 0; cntr < numVertices; cntr++)
   {
      vertices[cntr].x *= scale;
      vertices[cntr].y *= scale;
   }
}


void Shape::SetOrientation(CoordType _scaleX, CoordType _scaleY)
{
   for(uint32_t cntr = 0; cntr < numVertices; cntr++)
   {
      vertices[cntr].x *= _scaleX;
      vertices[cntr].y *= _scaleY;
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


void Shape::Reset()
{
   scale = 1;
   numVertices = 0;

   for(uint32_t cntr = 0; cntr < MAX_VERTICES; cntr++)
   {
      vertices[cntr] = { 0, 0, false };
   }
}


void Shape::CreateShape(int numeral)
{
   Reset();

   switch(numeral)
   {
      case ShapeTypeNumeral0:
         // Vertices for Shape '0'
         AddVertex(  2,  2, false);    // Start top right corner (don't draw going there
         AddVertex( -2,  2, true );    // Draw to top left corner
         AddVertex( -2, -2, true );    // Draw to bottom left corner
         AddVertex(  2, -2, true );    // Draw to bottom right corner
         AddVertex(  2,  2, true );    // Draw to top right corner
         AddVertex( -2, -2, true );    // Draw to bottom left corner (the slash through the zero

         // Reverse
         /*
         AddVertex(  2,  2, true );    // Draw to top right corner
         AddVertex(  2, -2, true );    // Draw to bottom right corner
         AddVertex( -2, -2, true );    // Draw to bottom left corner
         AddVertex( -2,  2, true );    // Draw to top left corner
         */
         break;

      case ShapeTypeNumeral1:
         // Vertices for Shape '1'
         AddVertex(  0,  2, false);
         AddVertex(  0, -2, true );     //  |
         break;

      case ShapeTypeNumeral2:
         // Vertices for Shape '2'
         AddVertex( -2,  2, false);    // Start top left
         AddVertex(  2,  2, true );    //  -
         AddVertex(  2,  0, true );    //   |
         AddVertex( -2,  0, true );    //  -
         AddVertex( -2, -2, true );    // |
         AddVertex(  2, -2, true );    //  -

         /* Draw in reverse too
         AddVertex( -2, -2, true );    // |
         AddVertex( -2,  0, true );    //  -
         AddVertex(  2,  0, true );    //   |
         AddVertex(  2,  2, true );    //  -
         */
         break;

      case ShapeTypeNumeral3:
         AddVertex( -2,  2, false);    // Start top left
         AddVertex(  2,  2, true );    //  -
         AddVertex(  2,  0, true );    //   |
         AddVertex(  0,  0, true );    //  -
         AddVertex(  2, -2, true );    //   |
         AddVertex(  2, -2, true );    //  -
         break;

      case ShapeTypeNumeral4:
         AddVertex( -2,  2, false);    // Start top left
         AddVertex( -2,  0, true );    //  | |
         AddVertex(  2,  0, true );    //  |-|
         AddVertex(  2,  2, true );    //    |
         AddVertex(  2, -2, true );    //    |
         break;

      case ShapeTypeNumeral5:
         AddVertex(  2,  2, false);    // Start top left
         AddVertex(  0,  2, true );    //   -
         AddVertex(  0,  0, true );    //  |
         AddVertex(  2,  0, true );    //   -
         AddVertex(  2, -2, true );    //    |
         AddVertex(  0, -2, true );    //   -
         break;

      case ShapeTypeNumeral6:
         AddVertex(  2,  2, false);    // Start top left
         AddVertex(  0,  2, true );    //   -
         AddVertex(  0, -2, true );    //  |
         AddVertex(  2, -2, true );    //   -
         AddVertex(  2,  0, true );    //  | |
         AddVertex(  0,  0, true );    //   -
         break;

      case ShapeTypeNumeral7:
         AddVertex(  0,  2, false);    // Start top left
         AddVertex(  2,  2, true );    //   --
         AddVertex(  0, -2, true );    //    /
                                       //   /
         break;

      case ShapeTypeNumeral8:
         AddVertex( -2,  2, false);    // Start top left
         AddVertex(  2, -2, true );    //  ---
         AddVertex( -2, -2, true );    //  \ /
         AddVertex(  2,  2, true );    //   x
         AddVertex( -2,  2, true );    //  / \
                                       //  ---
         break;

      case ShapeTypeNumeral9:
         AddVertex(  2, -2, false);    // Start bottom right
         AddVertex(  2,  2, true );    //   -
         AddVertex( -2,  2, true );    //  | |
         AddVertex( -2,  0, true );    //   -|
         AddVertex(  2,  0, true );    //    |
         break;

      case ShapeTypeCenterViewCal:
         AddVertex( -1,  0, true);
         AddVertex(  1,  0, true);
         break;

      case ShapeTypePaddle:
         AddVertex( 0,  2, true);      // Start Top middle
         AddVertex( 0, -2, true);      //   |
         break;

      case ShapeTypeBall:     // TODO: Still need to calculate vertices for a ball -- up to 16
         break;

      default:
         break;
   }
}

