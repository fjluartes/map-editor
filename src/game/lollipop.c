/*
 * Copyright (C) 2022 Parallel Realities. All rights reserved.
 */

#include "../common.h"

#include "../system/atlas.h"
#include "../system/draw.h"
#include "effects.h"
#include "lollipop.h"

extern App   app;
extern Stage stage;

static void tick(Entity *self);
static void draw(Entity *self);
static void touch(Entity *self, Entity *other);

static void initLollipop(Entity *e, char *filename)
{
	Item *item;

	item = malloc(sizeof(Item));
	memset(item, 0, sizeof(Item));

	item->bobValue = rand() % FPS;

	e->texture = getAtlasImage(filename, 1);
	e->w = e->texture->rect.w;
	e->h = e->texture->rect.h;
	e->flags = EF_WEIGHTLESS;

	e->data = item;

	e->tick = tick;
	e->draw = draw;
	e->touch = touch;

	stage.totalLollipops++;
}

void initRedLollipop(Entity *e)
{
	initLollipop(e, "gfx/entities/redLollipop.png");
}

void initYellowLollipop(Entity *e)
{
	initLollipop(e, "gfx/entities/yellowLollipop.png");
}

void initGreenLollipop(Entity *e)
{
	initLollipop(e, "gfx/entities/greenLollipop.png");
}

void initBlueLollipop(Entity *e)
{
	initLollipop(e, "gfx/entities/blueLollipop.png");
}

void initPurpleLollipop(Entity *e)
{
	initLollipop(e, "gfx/entities/purpleLollipop.png");
}

void initRandomLollipop(Entity *e)
{
	if (app.dev.isEditor)
	{
		initLollipop(e, "gfx/entities/randomLollipop.png");
	}
	else
	{
		switch (rand() % 5)
		{
			case 0:
				initRedLollipop(e);
				break;

			case 1:
				initYellowLollipop(e);
				break;

			case 2:
				initGreenLollipop(e);
				break;

			case 3:
				initBlueLollipop(e);
				break;

			default:
				initPurpleLollipop(e);
				break;
		}
	}
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

static void touch(Entity *self, Entity *other)
{
	if (other == stage.player && !self->dead)
	{
		stage.numLollipops++;

		self->dead = 1;

		addStarEffects(self->x + (self->w / 2), self->y + (self->h / 2), 25);
	}
}
