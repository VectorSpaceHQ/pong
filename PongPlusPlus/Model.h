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
      GameStateInit,
      GameStateCalibrateLasers,
      GameStateCalibrateView,
      GameStateReady,
      GameStatePlay,
      GameStateGameOver
   };

   enum WhoServes
   {
      LeftPlayerServes  = 0,
      RightPlayerServes,
      EitherPlayerServes
   };

   /***
    *
    */
   struct LaserCalibration
   {
   public:
      int32_t xOffset;
      int32_t yOffset;
      int32_t xOrientation;
      int32_t yOrientation;

      LaserCalibration():
         xOffset(0),
         yOffset(0),
         xOrientation(1),
         yOrientation(1)
      {
      }
   };


   /***
    * The DisplaySetings structure contains configuration settings for the display
    */
   struct DisplaySettings
   {
   public:
      int32_t  xMin;
      int32_t  xMax;
      int32_t  yMin;
      int32_t  yMax;
      uint32_t hSkew;   // TODO: Should skew be an integer where 0 is center?
      uint32_t vSkew;

      DisplaySettings():
         xMin(-200),
         yMin(-200),
         xMax(200),
         yMax(200),
         hSkew(0),
         vSkew(0)
      {
      }
   };


   struct Settings
   {
      LaserCalibration  leftLaserCal;
      LaserCalibration  middleLaserCal;
      LaserCalibration  rightLaserCal;

      DisplaySettings   display;
   };


   /***
    * The GameStatus configuration structure holds configuration information about the game
    * This configuration is controlled by the engine and visible by the view
    */
   struct GameStatus
   {

   public:
      GameState   gameState;
      bool        gameStateChanged;
      bool        viewSettingsChanged;
      Score       leftPaddleScore;
      Score       rightPaddleScore;
      WhoServes   whoseServe;

      Shape       leftPaddleShape;
      Shape       rightPaddleShape;
      Shape       ballShape;

      GameStatus():
         gameState(GameStateInit),
         gameStateChanged(false),
         viewSettingsChanged(false),
         leftPaddleScore(0),
         rightPaddleScore(0),
         whoseServe(EitherPlayerServes),
         leftPaddleShape(),
         rightPaddleShape(),
         ballShape()
      {
      }
   };

}


#endif   // __model_h__
