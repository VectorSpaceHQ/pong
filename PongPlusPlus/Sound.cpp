/*
 * Sound.cpp
 *
 *  Created on: Aug 27, 2018
 *      Author: aspontarelli
 *
 *  Information on Pong audio found here:
 *  http://cs.au.dk/~dsound/DigitalAudio.dir/Greenfoot/Pong.dir/Pong.html
 */

#include <Arduino.h>

void PlayWallSound(void){
  tone(13, 226, 16); // pin, hz, duration
}

void PlayPaddleSound(void){
  tone(13, 459, 96);
}

void PlayPointSound(void){
  tone(13, 490, 257);
}
