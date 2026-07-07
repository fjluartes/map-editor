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
#include "../system/text.h"
#include "editor.h"

#define ENTITY_SPACING 8

enum
{
	MODE_TITLES,
	MODE_ENTITIES
};

extern App   app;
extern Stage stage;

static void loadTiles(void);
static void addDefaultEntities(void);
static void logic(void);
static void draw(void);
static void doMouse(void);
static void doKeyboard(void);
static void cycleTile(int *i, int dir);
static void drawUI(void);
static void drawTopBar(void);
static void drawBottomBar(void);
static char *getTileTypeName(void);

static SDL_Point   mouseTile;
static int         currentTile;
static AtlasImage *tiles[MAX_TILES];
static AtlasImage *activeObjectArrowTexture;
static double	   activeObjectArrowBob;
static Entity 	  *currentEntity;
static Entit 	 **entities;
static int         currentEntityIndex;
static int         totalEntities;
static int         mode;
static char       *modeText[] = {"Tiles", "Entities"};

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

	mode = MODE_TILES;

	entities = getEditorEntities(&totalEntities);

	currentEntity = entities[0];

	activeObjectArrowTexture = getAtlasImage("gfx/editor/activeObjectArrow.png", 1);

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

static void drawUI(void)
{
	drawTopBar();

	drawBottomBar();
}

static void drawTopBar(void)
{
	char text[MAX_LINE_LENGTH];

	drawRect(0, 0, SCREEN_WIDTH, 48, 0, 0, 0, 192);

	sprintf(text, "Pos: %d,%d", mouseTile.x, mouseTile.y);
	drawText(text, 10, 0, 255, 255, 255, TEXT_ALIGN_LEFT, 0);

	sprintf(text, "Tile type: %s", getTileTypeName());
	drawText(text, 800, 0, 255, 255, 255, TEXT_ALIGN_LEFT, 0);

	sprintf(text, "Map: %s", stage.name);
	drawText(text, SCREEN_WIDTH - 10, 0, 255, 255, 255, TEXT_ALIGN_RIGHT, 0);
}

static char *getTileTypeName(void)
{
	if (currentTile >= TILE_WATER)
	{
		return "Water";
	}
	else if (currentTile >= TILE_FOREGROUND)
	{
		return "Foreground";
	}
	else
	{
		return "Solid";
	}
}

static void drawBottomBar(void)
{
	int x, x2, i, j;

	drawRect(0, SCREEN_HEIGHT - 54, SCREEN_WIDTH,
			 54, 32, 32, 32, 255);

	x = (SCREEN_WIDTH - MAP_TILE_SIZE) / 2;
	x2 = x - (MAP_TILE_SIZE + 4);

	i = currentTile;
	j = currentTile;

	while (x < SCREEN_WIDTH)
	{
		blitAtlasImage(tiles[i], x, SCREEN_HEIGHT - 50, 0, SDL_FLIP_NONE);
		blitAtlasImage(tiles[j], x2, SCREEN_HEIGHT - 50, 0, SDL_FLIP_NONE);

		x += MAP_TILE_SIZE + 4;
		x2 -= MAP_TILE_SIZE + 4;

		cycleTile(&i, 1);
		cycleTile(&j, -1);
	}

	x = (SCREEN_WIDTH - MAP_TILE_SIZE) / 2;

	drawOutlineRect(x, SCREEN_HEIGHT - MAP_TILE_SIZE - 2, 
		MAP_TILE_SIZE, MAP_TILE_SIZE, 255, 255, 0, 255);

	blitAtlasImage(activeObjectArrowTexture, x + (MAP_TILE_SIZE / 2), 
		SCREEN_HEIGHT - 64 + (sin(activeObjectArrowBob) * 8), 1, SDL_FLIP_NONE);
}
