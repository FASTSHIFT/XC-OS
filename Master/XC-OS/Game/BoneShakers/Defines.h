#pragma once

#if _WIN32
#include <stdint.h>
#include <string.h>
#define PROGMEM
#define PSTR
#define pgm_read_byte(x) (*((uint8_t*)x))
#define pgm_read_word(x) (*((uint16_t*)x))
#define pgm_read_ptr(x) (*((uintptr_t*)x))
#define strlen_P(x) strlen(x)
#else
#include <avr/pgmspace.h>
//#define pgm_read_ptr pgm_read_word
#endif

#ifdef _WIN32
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64
#else
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64
#endif

#define INPUT_LEFT 1
#define INPUT_RIGHT 2
#define INPUT_UP 4
#define INPUT_DOWN 8
#define INPUT_A 16
#define INPUT_B 32

#define CAMERA_HEIGHT (FIXED_ONE /2)
#define MAP_WIDTH 16
#define MAP_HEIGHT 16
#define CELL_SIZE 1
#define NUM_PLAYERS 4
#define START_X 3
#define START_Z 7
#define CAMERA_FOLLOW_DISTANCE (FIXED_ONE / 2)
#define MAX_TURN_VELOCITY 3
#define PLAYER_SIZE 16

#define MAX_ROAD_SPEED 85
#define MAX_STEERING_SPEED 60
#define MAX_SIDE_SPEED 45

#define TEXTURE_SIZE_BYTES 64

#define BACKGROUND_SIZE 64
