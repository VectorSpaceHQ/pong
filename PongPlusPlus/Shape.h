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
   ShapeTypeBall,
   ShapeTypeDot
};


enum CoordSet
{
   CoordsWorld = 0,
   CoordsView
};


class Shape
{
public:
   Shape();
   Shape(const Shape& other);

   uint16_t    scale;
   uint16_t    viewScale;
   uint32_t    numVertices;
   Vertex      vertices[MAX_VERTICES];       // World vertices
   Vertex      viewVertices[MAX_VERTICES];   // View vertices

   // Vertices for movement
   Vertex      position;         // Where it currently is in normal world space
   Vertex      vector;           // The direction/scale of the X/Y vector
   uint32_t    acceleration;     // The current acceleration (future?)

   // Vertices for collision detection in world space
   Vertex      highestVertex;
   Vertex      lowestVertex;
   Vertex      leftMostVertex;
   Vertex      rightMostVertex;

   void Reset();
   void CreateShape(int numeral);
   void AddVertex(CoordType x, CoordType y, bool draw);
   void CopyVerticesToView();

   // Methods to operate on the vertices
   void Move(CoordSet set, CoordType diffX, CoordType diffY);
   void Scale(CoordSet set, CoordType _scale);
   void SetOrientation(CoordSet set, CoordType _scaleX, CoordType _scaleY);
   void Add(CoordSet set, CoordType x, CoordType y);

   /***
    * Algorithms for collision detection
    */
   bool CheckTop(CoordType top, Vertex& foundVertex);
   bool CheckBottom(CoordType bottom, Vertex& foundVertex);
   bool CheckLeft(CoordType left, Vertex& foundVertex);
   bool CheckRight(CoordType right, Vertex& foundVertex);

   int16_t  Height();
   int16_t  Width();

   void Log(CoordSet set);

private:
   void CopyVertices(const Shape& other);

   void SetExtremeVertices(void);
};


#endif   // __shape_h__
