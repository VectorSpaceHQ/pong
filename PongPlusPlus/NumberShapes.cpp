/*
 * NumberShapes.cpp
 *
 *  Created on: Aug 25, 2018
 *      Author: athiessen
 */

#include <Arduino.h>

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

NumberShapes::NumberShapes():
   numerals()
{
   // Vertices for Shape '0'
   numerals[0].AddVertex(  2,  2, false);    // Start top right corner (don't draw going there
   numerals[0].AddVertex( -2,  2, true );    // Draw to top left corner
   numerals[0].AddVertex( -2, -2, true );    // Draw to bottom left corner
   numerals[0].AddVertex(  2, -2, true );    // Draw to bottom right corner
   numerals[0].AddVertex(  2,  2, true );    // Draw to top right corner
   numerals[0].AddVertex( -2, -2, true );    // Draw to bottom left corner (the slash through the zero

   // Vertices for Shape '1'
   numerals[1].AddVertex(  0,  2, false);
   numerals[1].AddVertex(  0, -2, true);     //  |

   // Vertices for Shape '1'
   numerals[2].AddVertex( -2,  2, false);    // Start top left
   numerals[2].AddVertex(  2,  2, true );    //  -
   numerals[2].AddVertex(  2,  0, true );    //   |
   numerals[2].AddVertex( -2,  0, true );    //  -
   numerals[2].AddVertex( -2, -2, true );    // |
   numerals[2].AddVertex(  2, -2, true );    //  _

}


const Shape& NumberShapes::operator[](uint8_t numeral)
{
   return const_cast<const Shape&>(numerals[numeral]);
}

