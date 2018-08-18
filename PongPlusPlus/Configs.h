
#ifndef __configs_h__
#define __configs_h__

#include <stdint.h>

typedef uint8_t   Score;      // TODO: Is it ok to have a max score of 255?

/***
 * The GameStatus configuration structure holds configuration information about the game
 * This configuration is controlled by the engine and visible by the view
 */
struct GameStatus
{
   enum GameState
   {
      GameStateCalibrate,
      GameStateReady,
      GameStatePlay,
      GameStateGameOver
   };

public:
   GameState   gameState;
   Score       leftPaddleScore;
   Score       rightPaddleScore;

   GameStatus():
      gameState(GameStateCalibrate),   // Default start in calibration mode
      leftPaddleScore(0),
      rightPaddleScore(0)
   {
   }
};


/***
 * The LaserConf configuration structure holds configuration information for a laser
 * including gimbal and laser control
 */
struct LaserConf
{
public:
   uint8_t xPin;
   uint8_t yPin;
   uint8_t laserPin;

   LaserConf(uint8_t _xPin, uint8_t _yPin, uint8_t _laserPin):
      xPin(_xPin),
      yPin(_yPin),
      laserPin(_laserPin)
   {
   }
};


/***
 * The PaddConf configuration structure holds configuration information for a paddle
 * controller (user input), including rotary encoder and push button
 */
struct PaddleConf
{
public:
   uint8_t  upPin;
   uint8_t  downPin;
   uint8_t  buttonPin;

   PaddleConf(uint8_t _upPin, uint8_t _downPin, uint8_t _buttonPin):
      upPin(_upPin),
      downPin(_downPin),
      buttonPin(_buttonPin)
   {
   }
};

#endif    // __configs_h__
