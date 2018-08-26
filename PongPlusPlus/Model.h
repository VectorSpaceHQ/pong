/*
 * Model.h
 *
 *  Created on: Aug 17, 2018
 *      Author: athiessen
 */

#ifndef __model_h__
#define __model_h__

#include <stdint.h>

#include "Shape.h"

namespace Model
{

   typedef uint8_t   Score;      // TODO: Is it ok to have a max score of 255?

   enum GameState
   {
      GameStateCalibrate,
      GameStateReady,
      GameStatePlay,
      GameStateGameOver
   };

   /***
    * The DisplaySetings structure contains configuration settings for the display
    */
   struct DisplaySettings
   {
   public:
      uint32_t xMin;
      uint32_t xMax;
      uint32_t yMin;
      uint32_t yMax;
      uint32_t hSkew;   // TODO: Should skew be an integer where 0 is center?
      uint32_t vSkew;

      DisplaySettings():
         xMin(0),
         yMin(0),
         xMax(800),
         yMax(600),
         hSkew(0),
         vSkew(0)
      {
      }
   };


   /***
    * The GameStatus configuration structure holds configuration information about the game
    * This configuration is controlled by the engine and visible by the view
    */
   struct GameStatus
   {

   public:
      GameState   gameState;
      Score       leftPaddleScore;
      Score       rightPaddleScore;

      Shape       leftPaddleShape;
      Shape       rightPaddleShape;
      Shape       ballShape;

      GameStatus():
         gameState(GameStateCalibrate),   // Default start in calibration mode
         leftPaddleScore(0),
         rightPaddleScore(0),
         leftPaddleShape(),
         rightPaddleShape(),
         ballShape()
      {
      }
   };

}


#endif   // __model_h__
