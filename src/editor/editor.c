/*
 * Copyright (C) 2022 Parallel Realities. All rights reserved.
 * 2026-07-09 fjluartes
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
#include "../system/util.h"
#include "editor.h"

#define ENTITY_SPACING 8
#define SCROLL_OVERSCAN (MAP_TILE_SIZE * 8)

enum
{
	MODE_TILES,
	MODE_ENTITIES,
	MODE_PICK
};

extern App   app;
extern Stage stage;

static void    loadTiles(void);
static void    addDefaultEntities(void);
static void    logic(void);
static void    draw(void);
static void    doMouse(void);
static void    doKeyboard(void);
static void    cycleTile(int *i, int dir);
static void    cycleEntity(int *i, int dir);
static Entity *findExisting(char *typeName);
static void    addEntity(void);
static void    removeEntity(void);
static void    pickEntity(void);
static void    drawUI(void);
static void    drawTopBar(void);
static void    drawBottomBar(void);
static char   *getTileTypeName(void);

static double      moveTimer;
static SDL_Point   mouseTile;
static int         currentTile;
static AtlasImage *tiles[MAX_TILES];
static AtlasImage *activeObjectArrowTexture;
static double      activeObjectArrowBob;
static Entity     *currentEntity;
static Entity    **entities;
static int         currentEntityIndex;
static int         totalEntities;
static int         mode;
static char       *modeText[] = {"Tiles", "Entities", "Pick"};

void initEditor(void)
{
	app.dev.isEditor = 1;

	SDL_ShowCursor(1);

	initBackground();

	loadTiles();

	initEntities();

	initMap();

	addDefaultEntities();

	moveTimer = 0;

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

		e = initEntity("startFlag");
		e->x = 150;
		e->y = 300;

		e = initEntity("endFlag");
		e->x = 250;
		e->y = 300;
	}
}

static void logic(void)
{
	mouseTile.x = (app.mouse.x + stage.camera.x) / MAP_TILE_SIZE;
	mouseTile.y = (app.mouse.y + stage.camera.y) / MAP_TILE_SIZE;

	if (currentEntity != NULL)
	{
		currentEntity->x = (int)((app.mouse.x + stage.camera.x) / ENTITY_SPACING);
		currentEntity->x *= ENTITY_SPACING;

		currentEntity->y = (int)((app.mouse.y + stage.camera.y) / ENTITY_SPACING);
		currentEntity->y *= ENTITY_SPACING;
	}

	moveTimer = MAX(moveTimer - app.deltaTime, 0);

	doMouse();

	doKeyboard();

	activeObjectArrowBob += 0.1 * app.deltaTime;
}

static void doMouse(void)
{
	if (isInsideMap(mouseTile.x, mouseTile.y))
	{
		if (mode == MODE_TILES)
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

		else if (mode == MODE_ENTITIES)
		{
			if (app.mouse.buttons[SDL_BUTTON_LEFT])
			{
				app.mouse.buttons[SDL_BUTTON_LEFT] = 0;

				addEntity();
			}

			if (app.mouse.buttons[SDL_BUTTON_RIGHT])
			{
				app.mouse.buttons[SDL_BUTTON_LEFT] = 0;

				removeEntity();
			}

			if (app.mouse.buttons[SDL_BUTTON_X1])
			{
				app.mouse.buttons[SDL_BUTTON_X1] = 0;

				cycleEntity(&currentEntityIndex, -1);

				currentEntity = entities[currentEntityIndex];
			}

			if (app.mouse.buttons[SDL_BUTTON_X2])
			{
				app.mouse.buttons[SDL_BUTTON_X2] = 0;

				cycleEntity(&currentEntityIndex, 1);

				currentEntity = entities[currentEntityIndex];
			}
		}
		else
		{
			if (app.mouse.buttons[SDL_BUTTON_LEFT])
			{
				app.mouse.buttons[SDL_BUTTON_LEFT] = 0;

				if (currentEntity == NULL)
				{
					pickEntity();
				}
				else
				{
					currentEntity = NULL;
				}
			}
		}
		
	}
}

static void addEntity(void)
{
	Entity *e;

	if (currentEntity->editorFlags & EMF_UNIQUE)
	{
		e = findExisting(currentEntity->typeName);
	}
	else
	{
		e = initEntity(currentEntity->typeName);
	}

	e->x = currentEntity->x;
	e->y = currentEntity->y;
}

static Entity *findExisting(char *typeName)
{
	Entity *e;

	for (e = stage.entityHead.next; e != NULL; e = e->next)
	{
		if (strcmp(e->typeName, typeName) == 0)
		{
			return e;
		}
	}

	return currentEntity;
}

static void removeEntity(void)
{
	Entity *e, *prev;

	prev = &stage.entityHead;

	for (e = stage.entityHead.next; e != NULL; e = e->next)
	{
		if (!(e->editorFlags & EMF_REQUIRED) &&
			collision(app.mouse.x + stage.camera.x, 
					  app.mouse.y + stage.camera.y, 
					  1, 1, e->x, e->y, e->w, e->h))
		{
			prev->next = e->next;

			if (e == stage.entityTail)
			{
				stage.entityTail = prev;
			}

			if (e->data != NULL)
			{
				free(e->data);
			}

			free(e);

			e = prev;
		}

		prev = e;
	}
}

static void pickEntity(void)
{
	Entity *e;

	for (e = stage.entityHead.next; e != NULL; e = e->next)
	{
		if (collision(app.mouse.x + stage.camera.x,
					  app.mouse.y + stage.camera.y,
					  1, 1,
					  e->x, e->y, e->w, e->h))
		{
			currentEntity = e;
			return;
		}
	}
}

static void doKeyboard(void)
{
	int dx, dy;

	if (moveTimer <= 0)
	{
		dx = dy = 0;

		if (app.keyboard[SDL_SCANCODE_A])
		{
			dx = -MAP_TILE_SIZE;
		}

		if (app.keyboard[SDL_SCANCODE_D])
		{
			dx = MAP_TILE_SIZE;
		}

		if (app.keyboard[SDL_SCANCODE_W])
		{
			dy = -MAP_TILE_SIZE;
		}

		if (app.keyboard[SDL_SCANCODE_D])
		{
			dy = MAP_TILE_SIZE;
		}

		if (dx != 0 || dy != 0)
		{
			stage.camera.x = MIN(MAX(stage.camera.x + dx, -SCROLL_OVERSCAN),
								 (MAP_WIDTH * MAP_TILE_SIZE) - SCROLL_OVERSCAN);
			stage.camera.y = MIN(MAX(stage.camera.y + dy, -SCROLL_OVERSCAN),
								 (MAP_WIDTH * MAP_TILE_SIZE) - SCROLL_OVERSCAN);
			moveTimer = 3;
		}
	}

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

	if (app.keyboard[SDL_SCANCODE_1])
	{
		app.keyboard[SDL_SCANCODE_1] = 0;

		mode = MODE_TILES;

		currentEntity = NULL;
	}

	if (app.keyboard[SDL_SCANCODE_2])
	{
		app.keyboard[SDL_SCANCODE_2] = 0;

		mode = MODE_ENTITIES;

		currentEntity = entities[currentEntityIndex];
	}

	if (app.keyboard[SDL_SCANCODE_3])
	{
		app.keyboard[SDL_SCANCODE_3] = 0;

		mode = MODE_PICK;

		currentEntity = NULL;
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

static void cycleEntity(int *i, int dir)
{
	*i = *i + dir;

	if (*i < 0)
	{
		*i = totalEntities - 1;
	}

	if (*i >= totalEntities)
	{
		*i = 0;
	}
}

static void draw(void)
{
	drawBackground();

	if (mode == MODE_ENTITIES)
	{
		drawGridLines(ENTITY_SPACING);
	}

	drawEntities(LAYER_BACKGROUND);

	drawMap(LAYER_BACKGROUND);

	drawEntities(LAYER_FOREGROUND);

	drawMap(LAYER_FOREGROUND);

	if (mode == MODE_TILES)
	{
		drawGridLines(MAP_TILE_SIZE);

		blitAtlasImage(tiles[currentTile], 
					   (mouseTile.x * MAP_TILE_SIZE) - stage.camera.x, 
					   (mouseTile.y * MAP_TILE_SIZE) - stage.camera.y, 
					   0, SDL_FLIP_NONE);

		drawOutlineRect((mouseTile.x * MAP_TILE_SIZE) - stage.camera.x, 
						(mouseTile.y * MAP_TILE_SIZE) - stage.camera.y, 
						MAP_TILE_SIZE, MAP_TILE_SIZE, 255, 255, 0, 255);
	}
	else if (currentEntity != NULL)
	{
		currentEntity->draw(currentEntity);

		drawOutlineRect(currentEntity->x - stage.camera.x, 
						currentEntity->y - stage.camera.y, 
						currentEntity->w, currentEntity->h, 255, 255, 0, 255);
	}
	

	drawUI();
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

	if (mode == MODE_TILES)
	{
		sprintf(text, "Pos: %d,%d", mouseTile.x, mouseTile.y);	
	}
	else if (currentEntity != NULL)
	{
		sprintf(text, "Pos: %d,%d", 
				(int)currentEntity->x, (int)currentEntity->y);
	}
	else
	{
		sprintf(text, "Pos, -,-");
	}

	drawText(text, 10, 0, 255, 255, 255, TEXT_ALIGN_LEFT, 0);

	sprintf(text, "Mode, %s", modeText[mode]);
	drawText(text, 400, 0, 255, 255, 255, TEXT_ALIGN_LEFT, 0);

	if (mode == MODE_TILES) 
	{
		sprintf(text, "Tile type: %s", getTileTypeName());
		drawText(text, 800, 0, 255, 255, 255, TEXT_ALIGN_LEFT, 0);
	}
	else if (currentEntity != NULL)
	{
		sprintf(text, "Entity: %s", currentEntity->typeName);
		drawText(text, 800, 0, 255, 255, 255, TEXT_ALIGN_LEFT, 0);
	}

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

	x = 0;

	x = (SCREEN_WIDTH - MAP_TILE_SIZE) / 2;
	x2 = x - (MAP_TILE_SIZE + 4);

	if (mode == MODE_TILES)
	{
		i = currentTile;
		j = currentTile;

		cycleTile(&j, -1);

		while (x < SCREEN_WIDTH)
		{
			blitAtlasImage(tiles[i], x, SCREEN_HEIGHT - 50, 0, SDL_FLIP_NONE);
			blitAtlasImage(tiles[j], x2, SCREEN_HEIGHT - 50, 0, SDL_FLIP_NONE);

			x += MAP_TILE_SIZE + 4;
			x2 -= MAP_TILE_SIZE + 4;

			cycleTile(&i, 1);
			cycleTile(&j, -1);
		}
	}
	else if (mode == MODE_ENTITIES)
	{
		i = currentEntityIndex;
		j = currentEntityIndex;

		cycleEntity(&j, -1);

		while (x < SCREEN_WIDTH)
		{
			blitAtlasImage(entities[i]->texture, x, 
						   SCREEN_HEIGHT - 50, 0, SDL_FLIP_NONE);
			blitAtlasImage(entities[j]->texture, x2, 
						   SCREEN_HEIGHT - 50, 0, SDL_FLIP_NONE);

			x += MAP_TILE_SIZE + 4;
			x2 -= MAP_TILE_SIZE + 4;

			cycleEntity(&i, 1);
			cycleEntity(&j, -1);
		}
	}

	if (mode != MODE_PICK)
	{
		x = (SCREEN_WIDTH - MAP_TILE_SIZE) / 2;

		drawOutlineRect(x, SCREEN_HEIGHT - MAP_TILE_SIZE - 2, 
						MAP_TILE_SIZE, MAP_TILE_SIZE, 255, 255, 0, 255);

		blitAtlasImage(activeObjectArrowTexture, x + (MAP_TILE_SIZE / 2), 
					   SCREEN_HEIGHT - 64 + (sin(activeObjectArrowBob) * 8), 
					   1, SDL_FLIP_NONE);
	}
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
