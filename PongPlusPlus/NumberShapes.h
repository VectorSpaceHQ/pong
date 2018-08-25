/*
 * NumberShapes.h
 *
 *  Created on: Aug 24, 2018
 *      Author: athiessen
 */

#ifndef __number_shapes_h
#define __number_shapes_h

#include "Shape.h"

#define  MAX_NUMERALS   (2)

class NumberShapes
{
private:
   static Vertex  numeralVertices[MAX_NUMERALS][MAX_VERTICES];
   static Shape   numerals[MAX_NUMERALS];

public:
   NumberShapes();

   const Shape& operator[](uint8_t numeral);

};


#endif   // __number_shapes_h
