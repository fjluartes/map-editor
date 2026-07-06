/*
 * Copyright (C) 2022 Parallel Realities. All rights reserved.
 */

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define STRCPY(dest, src)             \
	strncpy(dest, src, sizeof(dest)); \
	dest[sizeof(dest) - 1] = '\0'
#define STRNCPY(dest, src, n) \
	strncpy(dest, src, n);    \
	dest[n - 1] = '\0'

#define PI 3.14159265358979323846

#define SCREEN_WIDTH  1600
#define SCREEN_HEIGHT 900

#define FPS        60
#define LOGIC_RATE (FPS * 1.0 / 1000)

#define MAX_NAME_LENGTH        32
#define MAX_DESCRIPTION_LENGTH 256
#define MAX_FILENAME_LENGTH    256
#define MAX_LINE_LENGTH        1024

#define NUM_ATLAS_BUCKETS 64

#define MAX_MOUSE_BUTTONS 8

#define MAX_KEYBOARD_KEYS 350

#define MAX_SND_CHANNELS 16

#define MAP_TILE_SIZE 48

#define MAX_TILES 200

#define MAP_WIDTH  33
#define MAP_HEIGHT 18

#define LAYER_BACKGROUND 0
#define LAYER_FOREGROUND 1

#define TILE_SOLID      1
#define TILE_FOREGROUND 100
#define TILE_WATER      150

#define MAP_RENDER_WIDTH  ((SCREEN_WIDTH / MAP_TILE_SIZE) + 1)
#define MAP_RENDER_HEIGHT ((SCREEN_HEIGHT / MAP_TILE_SIZE) + 1)

#define MAP_DATA_PATH "data/maps"

#define EF_NONE       0
#define EF_WEIGHTLESS (2 << 0)
#define EF_SOLID      (2 << 1)
#define EF_PUSH       (2 << 2)
#define EF_IN_WATER   (2 << 3)

#define EMF_NONE     0
#define EMF_UNIQUE   (2 << 0)
#define EMF_REQUIRED (2 << 1)

enum
{
	TEXT_ALIGN_LEFT,
	TEXT_ALIGN_CENTER,
	TEXT_ALIGN_RIGHT
};

enum
{
	FACING_LEFT,
	FACING_RIGHT
};
