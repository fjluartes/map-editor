/*
 * Copyright (C) 2022 Parallel Realities. All rights reserved.
 */

#include "../common.h"

#include "../system/atlas.h"
#include "../system/draw.h"
#include "../system/util.h"
#include "map.h"

#define MAP_DATA_FILENAME_LENGTH (MAX_FILENAME_LENGTH + 4)

extern App   app;
extern Stage stage;

static void  loadTiles(void);
static char *getMapFilename(void);

static AtlasImage *tiles[MAX_TILES];
static AtlasImage *checkerboard;

void initMap(void)
{
	loadTiles();

	checkerboard = getAtlasImage("gfx/editor/checkerboard.png", 1);

	loadMap();
}

void drawMap(int layer)
{
	int x, y, n, x1, x2, y1, y2, mx, my;

	x1 = ((int)stage.camera.x % MAP_TILE_SIZE) * -1;
	x2 = x1 + MAP_RENDER_WIDTH * MAP_TILE_SIZE + (x1 == 0 ? 0 : MAP_TILE_SIZE);

	y1 = ((int)stage.camera.y % MAP_TILE_SIZE) * -1;
	y2 = y1 + MAP_RENDER_HEIGHT * MAP_TILE_SIZE + (y1 == 0 ? 0 : MAP_TILE_SIZE);

	mx = stage.camera.x / MAP_TILE_SIZE;
	my = stage.camera.y / MAP_TILE_SIZE;

	for (y = y1; y < y2; y += MAP_TILE_SIZE)
	{
		for (x = x1; x < x2; x += MAP_TILE_SIZE)
		{
			if (isInsideMap(mx, my))
			{
				n = stage.map[mx][my];

				if ((n > 0) && ((layer == LAYER_BACKGROUND && n < 100) || (layer == LAYER_FOREGROUND && n >= 100)))
				{
					blitAtlasImage(tiles[n], x, y, 0, SDL_FLIP_NONE);
				}
			}
			else if (app.dev.isEditor)
			{
				blitAtlasImage(checkerboard, x, y, 0, SDL_FLIP_NONE);
			}

			mx++;
		}

		mx = stage.camera.x / MAP_TILE_SIZE;

		my++;
	}
}

int isInsideMap(int x, int y)
{
	return x >= 0 && y >= 0 && x < MAP_WIDTH && y < MAP_HEIGHT;
}

int isSolidMap(int x, int y)
{
	return stage.map[x][y] >= TILE_SOLID && stage.map[x][y] < TILE_FOREGROUND;
}

int isWater(int x, int y)
{
	return stage.map[x][y] >= TILE_WATER;
}

void loadMap(void)
{
	int   x, y;
	char *data, *p, *filename;

	filename = getMapFilename();

	if (fileExists(filename))
	{
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading map %s ...\n", filename);

		data = readFile(filename);

		p = data;

		for (y = 0; y < MAP_HEIGHT; y++)
		{
			for (x = 0; x < MAP_WIDTH; x++)
			{
				stage.map[x][y] = atoi(p);

				do
				{
					p++;
				} while (isdigit(*p));
			}
		}

		free(data);
	}
	else if (!app.dev.isEditor)
	{
		printf("Couldn't load '%s'\n", filename);
		free(filename);
		exit(1);
	}

	free(filename);
}

int saveMap(void)
{
	FILE *fp;
	char *filename;
	int   x, y, rtn;

	filename = getMapFilename();

	fp = fopen(filename, "w");

	if (fp)
	{
		for (y = 0; y < MAP_HEIGHT; y++)
		{
			for (x = 0; x < MAP_WIDTH; x++)
			{
				fprintf(fp, "%d ", stage.map[x][y]);
			}
		}

		fclose(fp);

		rtn = 1;
	}
	else
	{
		rtn = 0;
	}

	free(filename);

	return rtn;
}

static char *getMapFilename(void)
{
	return formattedString("data/maps/%s.dat", stage.name);
}

static void loadTiles(void)
{
	int  i;
	char filename[MAX_FILENAME_LENGTH];

	for (i = 0; i < MAX_TILES; i++)
	{
		sprintf(filename, "gfx/tiles/%d.png", i);

		tiles[i] = getAtlasImage(filename, 0);
	}
}
