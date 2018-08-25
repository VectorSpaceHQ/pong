/*
 * NumberShapes.cpp
 *
 *  Created on: Aug 25, 2018
 *      Author: athiessen
 */


#include "NumberShapes.h"

/***
 * This structure defines the vertices of all the various numerals,
 * using a 5x5 grid with 0 in the middle
 *
 * e.g. The numeral 0:
 *             Y
 *    X-Axis:   -2 -1  0  1  2
 *            2  *           *
 *            1
 *            0
 *           -1
 *           -2 *            *
 *
 */
Vertex NumberShapes::numeralVertices[MAX_NUMERALS][MAX_VERTICES] =
{
   {
      // Vertices for Shape '0'
      {  2,  2, false },   // Start top right corner (don't draw going there
      { -2,  2, true  },   // Draw to top left corner
      { -2, -2, true  },   // Draw to bottom left corner
      {  2, -2, true  },   // Draw to bottom right corner
      {  2,  2, true  },   // Draw to top right corner
      { -2, -2, true  },   // Draw to bottom left corner (the slash through the zero
      {  0,  0, false },
      {  0,  0, false },
      {  0,  0, false },
      {  0,  0, false },
      {  0,  0, false },
      {  0,  0, false },
      {  0,  0, false },
      {  0,  0, false },
      {  0,  0, false },
      {  0,  0, false }
   },
   {
      // Vertices for Shape '1'
      {  0,  2, false },
      {  0, -2, true  },
      {  0,  0, false },
      {  0,  0, false },
      {  0,  0, false },
      {  0,  0, false },
      {  0,  0, false },
      {  0,  0, false },
      {  0,  0, false },
      {  0,  0, false },
      {  0,  0, false },
      {  0,  0, false },
      {  0,  0, false },
      {  0,  0, false },
      {  0,  0, false },
      {  0,  0, false }
   }
};


NumberShapes::NumberShapes()
{
   numerals[0] = Shape(6, &numeralVertices[0][0]);
   numerals[1] = Shape(2, &numeralVertices[1][0]);
}


