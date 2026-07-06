/*
 * Copyright (C) 2022 Parallel Realities. All rights reserved.
 */

#include "../common.h"

#include "../system/atlas.h"
#include "../system/draw.h"
#include "effects.h"
#include "keys.h"

extern App   app;
extern Stage stage;

static void tick(Entity *self);
static void draw(Entity *self);
static void touchBronze(Entity *self, Entity *other);
static void touchBlue(Entity *self, Entity *other);

static void initKey(Entity *e, char *texture)
{
	Item *item;

	item = malloc(sizeof(Item));
	memset(item, 0, sizeof(Item));

	item->bobValue = rand() % FPS;

	e->texture = getAtlasImage(texture, 1);
	e->w = e->texture->rect.w;
	e->h = e->texture->rect.h;
	e->flags = EF_WEIGHTLESS;
	e->data = item;

	e->tick = tick;
	e->draw = draw;
}

void initBronzeKey(Entity *e)
{
	initKey(e, "gfx/entities/bronzeKey.png");

	e->touch = touchBronze;
}

void initBlueKey(Entity *e)
{
	initKey(e, "gfx/entities/blueKey.png");

	e->touch = touchBlue;
}

static void tick(Entity *self)
{
	((Item *)self->data)->bobValue += 0.05 * app.deltaTime;
}

static void draw(Entity *self)
{
	double y;

	y = self->y - stage.camera.y;

	if (!app.dev.isEditor)
	{
		y += sin(((Item *)self->data)->bobValue) * 8;
	}

	blitAtlasImage(self->texture, self->x - stage.camera.x, y, 0, SDL_FLIP_NONE);
}

static void touchBronze(Entity *self, Entity *other)
{
	if (!self->dead && other == stage.player)
	{
		stage.bronzeKeys++;

		self->dead = 1;

		addStarEffects(self->x + (self->w / 2), self->y + (self->h / 2), 50);
	}
}

static void touchBlue(Entity *self, Entity *other)
{
	if (!self->dead && other == stage.player)
	{
		stage.blueKeys++;

		self->dead = 1;

		addStarEffects(self->x + (self->w / 2), self->y + (self->h / 2), 50);
	}
}
