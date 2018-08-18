/*
 * Timing.h
 *
 *  Created on: Aug 18, 2018
 *      Author: athiessen
 */

#ifndef __timing_h__
#define __timing_h__


#define MAIN_LOOP_TIME              (5)      // 5 microseconds
#define ENGINE_LOOP_INTERVAL        (3333)   // Run the engine at 30fps  3333 * 5 us = 0.016665s ~= 60hz ~= 30fps
#define VIEW_LOOP_INTERVAL          (1)      // Run the view every time for now for laser scheduling
#define CONTROLLER_LOOP_INTERVAL    (1)      // Run the controller every time for now for button debounce, etc.


#endif   // __timing_h__
