/*
 * Copyright (C) 2022 Parallel Realities. All rights reserved.
 * 2026-07-06 fjluartes
 */

#include <SDL_mixer.h>

#include "../common.h"

#include "../game/background.h"
#include "../game/entities.h"
#include "../game/entityFactory.h"
#include "../game/map.h"
#include "../json/cJSON.h"
#include "../system/atlas.h"
#include "../system/draw.h"
#include "editor.h"

extern App   app;
extern Stage stage;

static void loadTiles(void);
static void addDefaultEntities(void);
static void logic(void);
static void draw(void);
static void doMouse(void);
static void doKeyboard(void);
static void cycleTile(int *i, int dir);

static SDL_Point   mouseTile;
static int         currentTile;
static AtlasImage *tiles[MAX_TILES];
static SDL_Texture *activeObjectArrowTexture;
static int activeObjectArrowBob;

void initEditor(void)
{
	app.dev.isEditor = 1;

	SDL_ShowCursor(1);

	initBackground();

	loadTiles();

	initEntities();

	initMap();

	addDefaultEntities();

	currentTile = 1;

	activeObjectArrowTexture = getAtlasImage("gfx/editor/activeObjectArrow.png");

	activeObjectArrowBob = 0;

	app.delegate.logic = logic;
	app.delegate.draw = draw;
}

static void addDefaultEntities(void)
{
	Entity *e;

	if (stage.entityHead.next == NULL)
	{
		e = initEntity("player");
		e->x = 50;
		e->y = 300;
	}
}

static void logic(void)
{
	mouseTile.x = (app.mouse.x + stage.camera.x) / MAP_TILE_SIZE;
	mouseTile.y = (app.mouse.y + stage.camera.y) / MAP_TILE_SIZE;

	doMouse();

	doKeyboard();

	activeObjectArrowBob += 0.1 * app.deltaTime;
}

static void doMouse(void)
{
	if (isInsideMap(mouseTile.x, mouseTile.y))
	{
		if (app.mouse.buttons[SDL_BUTTON_LEFT])
		{
			stage.map[mouseTile.x][mouseTile.y] = currentTile;
		}

		if (app.mouse.buttons[SDL_BUTTON_RIGHT])
		{
			stage.map[mouseTile.x][mouseTile.y] = 0;
		}

		if (app.mouse.buttons[SDL_BUTTON_X1])
		{
			app.mouse.buttons[SDL_BUTTON_X1] = 0;

			cycleTile(&currentTile, -1);
		}

		if (app.mouse.buttons[SDL_BUTTON_X2])
		{
			app.mouse.buttons[SDL_BUTTON_X2] = 0;

			cycleTile(&currentTile, 1);
		}
	}
}

static void doKeyboard(void)
{
	if (app.keyboard[SDL_SCANCODE_SPACE])
	{
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, 
			"Saving map '%s' ... ", stage.name);

		if (!saveMap() || !saveEntities())
		{
			SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN, 
				"Failed to save map!");
		}
		else
		{
			SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, 
				"Saving map '%s' ... Done", stage.name);
		}

		app.keyboard[SDL_SCANCODE_SPACE] = 0;
	}
}

static void draw(void)
{
	drawBackground();

	drawEntities(LAYER_BACKGROUND);

	drawMap(LAYER_BACKGROUND);

	drawEntities(LAYER_FOREGROUND);

	drawMap(LAYER_FOREGROUND);

	drawGridLines(MAP_TILE_SIZE);

	blitAtlasImage(tiles[currentTile], 
		(mouseTile.x * MAP_TILE_SIZE) - stage.camera.x, 
		(mouseTile.y * MAP_TILE_SIZE) - stage.camera.y, 0, SDL_FLIP_NONE);

	drawOutlineRect((mouseTile.x * MAP_TILE_SIZE) - stage.camera.x, 
		(mouseTile.y * MAP_TILE_SIZE) - stage.camera.y, 
		MAP_TILE_SIZE, MAP_TILE_SIZE, 255, 255, 0, 255);

	drawUI();
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

static void cycleTile(int *i, int dir)
{
	do
	{
		*i = *i + dir;

		if (*i < 0)
		{
			*i = MAX_TILES - 1;
		}

		if (*i >= MAX_TILES)
		{
			*i = 1;
		}
	} while (tiles[*i] == NULL);
}
