/*
 * Copyright (C) 2022 Parallel Realities. All rights reserved.
 */

#include "../common.h"

#include "background.h"
#include "camera.h"
#include "effects.h"
#include "entities.h"
#include "hud.h"
#include "map.h"
#include "stage.h"

extern App   app;
extern Stage stage;

static void logic(void);
static void addPlayerToStart(int restoreToWorld);
static void draw(void);

static double resetTimer;

void initStage(void)
{
	initEntities();

	initMap();

	initBackground();

	initEffects();

	initHud();

	addPlayerToStart(0);

	resetTimer = FPS;

	app.delegate.logic = logic;
	app.delegate.draw = draw;
}

static void addPlayerToStart(int restoreToWorld)
{
	Entity *e;

	for (e = stage.entityHead.next; e != NULL; e = e->next)
	{
		if (strcmp(e->name, "Start Flag") == 0)
		{
			stage.player->dead = 0;
			stage.player->x = e->x;
			stage.player->y = e->y - 16;
			stage.player->dx = stage.player->dy = 0;
			stage.player->flags &= ~EF_IN_WATER;

			if (restoreToWorld)
			{
				stage.entityTail->next = stage.player;
			}
			return;
		}
	}
}

static void logic(void)
{
	doEntities();

	doEffects();

	doCamera();

	if (stage.player->dead)
	{
		resetTimer -= app.deltaTime;

		if (resetTimer <= 0)
		{
			addPlayerToStart(1);

			resetTimer = FPS;
		}
	}
}

static void draw(void)
{
	drawBackground();

	drawEntities(LAYER_BACKGROUND);

	drawMap(LAYER_BACKGROUND);

	drawEntities(LAYER_FOREGROUND);

	drawMap(LAYER_FOREGROUND);

	drawEffects();

	drawHud();
}
