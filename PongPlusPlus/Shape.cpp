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
   viewScale(1),
   numVertices(0),
   position(),
   vector(),
   highestVertex(),
   lowestVertex(),
   leftMostVertex(),
   rightMostVertex(),
   acceleration(0)
{
   Reset();
}


Shape::Shape(const Shape& other)
{
   if(this != &other)
   {
      scale = other.scale;
      viewScale = other.viewScale;
      CopyVertices(other);
   }
}


void Shape::Move(CoordSet set, CoordType diffX, CoordType diffY)
{
   // Add the difference to our coordinates
   Add(set, diffX, diffY);

   // Update our current position
   if(set == CoordsWorld)
   {
      position.x += diffX;
      position.y += diffY;
   }
}


void Shape::Scale(CoordSet set, CoordType _scale)
{
   if(set == CoordsWorld)
   {
      scale = _scale;

      for(uint32_t cntr = 0; cntr < numVertices; cntr++)
      {
         vertices[cntr].x *= scale;
         vertices[cntr].y *= scale;
      }

      SetExtremeVertices();
   }
   else
   {
      viewScale = _scale;

      for(uint32_t cntr = 0; cntr < numVertices; cntr++)
      {
         viewVertices[cntr].x *= scale;
         viewVertices[cntr].y *= scale;
      }
   }
}


void Shape::SetOrientation(CoordSet set, CoordType _scaleX, CoordType _scaleY)
{
   if(set == CoordsWorld)
   {
      for(uint32_t cntr = 0; cntr < numVertices; cntr++)
      {
         vertices[cntr].x *= _scaleX;
         vertices[cntr].y *= _scaleY;
      }

      SetExtremeVertices();
   }
   else
   {
      for(uint32_t cntr = 0; cntr < numVertices; cntr++)
      {
         viewVertices[cntr].x *= _scaleX;
         viewVertices[cntr].y *= _scaleY;
      }
   }
}


void Shape::Add(CoordSet set, CoordType x, CoordType y)
{
   if(set == CoordsWorld)
   {
      for(uint32_t cntr = 0; cntr < numVertices; cntr++)
      {
         vertices[cntr].x += x;
         vertices[cntr].y += y;
      }

      // Move our extreme vertices too
      highestVertex.x += x;
      highestVertex.y += y;

      lowestVertex.x += x;
      lowestVertex.y += y;

      leftMostVertex.x += x;
      leftMostVertex.y += y;

      rightMostVertex.x += x;
      rightMostVertex.y += y;
   }
   else
   {
      for(uint32_t cntr = 0; cntr < numVertices; cntr++)
      {
         viewVertices[cntr].x += x;
         viewVertices[cntr].y += y;
      }
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
      SetExtremeVertices();
   }
}


void Shape::CopyVerticesToView()
{
   for(uint32_t cntr = 0; cntr < numVertices; cntr++)
   {
      viewVertices[cntr].x = vertices[cntr].x;
      viewVertices[cntr].y = vertices[cntr].y;
      viewVertices[cntr].draw = vertices[cntr].draw;
   }
}


void Shape::CopyVertices(const Shape& other)
{
   numVertices = other.numVertices;

   if(numVertices > MAX_VERTICES)
   {
      numVertices = MAX_VERTICES;
   }

   for(uint32_t cntr = 0; cntr < numVertices; cntr++)
   {
      vertices[cntr] = other.vertices[cntr];
      viewVertices[cntr] = other.viewVertices[cntr];
   }

   SetExtremeVertices();
}


void Shape::Reset()
{
   scale = 1;
   viewScale = 1;
   numVertices = 0;

   for(uint32_t cntr = 0; cntr < MAX_VERTICES; cntr++)
   {
      vertices[cntr] = { 0, 0, false };
      viewVertices[cntr] = { 0, 0, false };
   }

   highestVertex.x =  0;
   highestVertex.y =  1000000;

   lowestVertex.x =  0;
   lowestVertex.y = -1000000;

   leftMostVertex.x = -1000000;
   leftMostVertex.y =  0;

   rightMostVertex.x = 1000000;
   rightMostVertex.y = 0;
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

   SetExtremeVertices();
}


/******************************************************************************
 * Collision detection - Helper algorithms
 ******************************************************************************/
void Shape::SetExtremeVertices(void)
{
   for(uint32_t cntr = 0; cntr < numVertices; cntr++)
   {
      if(vertices[cntr].y > highestVertex.y)
      {
         highestVertex.x = vertices[cntr].x;
         highestVertex.y = vertices[cntr].y;
      }

      if(vertices[cntr].y < lowestVertex.y)
      {
         lowestVertex.x = vertices[cntr].x;
         lowestVertex.y = vertices[cntr].y;
      }

      if(vertices[cntr].x > rightMostVertex.x)
      {
         rightMostVertex.x = vertices[cntr].x;
         rightMostVertex.y = vertices[cntr].y;
      }

      if(vertices[cntr].x < leftMostVertex.x)
      {
         leftMostVertex.x = vertices[cntr].x;
         leftMostVertex.y = vertices[cntr].y;
      }
   }
}


bool Shape::CheckTop(CoordType top, Vertex& foundVertex)
{
   bool isBeyond = false;

   if(highestVertex.y >= top)
   {
      foundVertex.x = highestVertex.x;
      foundVertex.y = highestVertex.y;

      isBeyond = true;
   }

   return isBeyond;
}


bool Shape::CheckBottom(CoordType bottom, Vertex& foundVertex)
{
   bool isBeyond = false;

   if(lowestVertex.y <= bottom)
   {
      foundVertex.x = lowestVertex.x;
      foundVertex.y = lowestVertex.y;

      isBeyond = true;
   }

   return isBeyond;
}


bool Shape::CheckLeft(CoordType left, Vertex& foundVertex)
{
   bool isBeyond = false;

   if(leftMostVertex.x <= left)
   {
      foundVertex.x = leftMostVertex.x;
      foundVertex.y = leftMostVertex.y;

      isBeyond = true;
   }

   return isBeyond;
}


bool Shape::CheckRight(CoordType right, Vertex& foundVertex)
{
   bool isBeyond = false;

   if(rightMostVertex.x >= right)
   {
      foundVertex.x = rightMostVertex.x;
      foundVertex.y = rightMostVertex.y;

      isBeyond = true;
   }

   return isBeyond;
}


bool Shape::CheckBetweenVertical(CoordType top, CoordType bottom)
{
}
