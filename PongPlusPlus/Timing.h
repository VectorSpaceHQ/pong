/*
 * Timing.h
 *
 *  Created on: Aug 18, 2018
 *      Author: athiessen
 */

#ifndef __timing_h__
#define __timing_h__


#define MAIN_LOOP_TIME              (100)       // microseconds
#define ENGINE_LOOP_INTERVAL        (167)       // Run the engine at 30fps  3333 * 5 us = 0.016665s ~= 60hz ~= 30fps
#define VIEW_LOOP_INTERVAL          (1)         // Run the view every time for now for laser scheduling

// TODO: Review controller timing!
#define CONTROLLER_LOOP_INTERVAL    (1)         // Run the controller every time for now for button debounce, etc.
#define PADDLE_CHECK_INTERVAL       (1)         // How often to check the status of the rotary switch
#define PADDLE_BUTTON_INCR_INTERVAL (1000 / MAIN_LOOP_TIME / CONTROLLER_LOOP_INTERVAL / PADDLE_CHECK_INTERVAL) // Convert paddle check status time to ms
#define PADDLE_UPDATE_INTERVAL      (ENGINE_LOOP_INTERVAL / 4)    // How often to update the status of the paddle from the controller


#endif   // __timing_h__
