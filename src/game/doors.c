/*
 * Copyright (C) 2022 Parallel Realities. All rights reserved.
 */

#include "../common.h"

#include "../system/atlas.h"
#include "../system/draw.h"
#include "doors.h"

extern App   app;
extern Stage stage;

static void tick(Entity *self);
static void draw(Entity *self);
static void touchNormal(Entity *self, Entity *other);
static void touchBronze(Entity *self, Entity *other);
static void touchBlue(Entity *self, Entity *other);
static void postLoad(Entity *self);

static void initDoor(Entity *e, char *texture)
{
	Door *d;

	d = malloc(sizeof(Door));
	memset(d, 0, sizeof(Door));

	e->texture = getAtlasImage(texture, 1);
	e->w = e->texture->rect.w;
	e->h = e->texture->rect.h;
	e->layer = LAYER_BACKGROUND;
	e->flags = EF_SOLID + EF_WEIGHTLESS;

	e->data = d;

	e->tick = tick;
	e->draw = draw;
	e->postLoad = postLoad;
}

void initNormalDoor(Entity *e)
{
	initDoor(e, "gfx/entities/door.png");

	e->touch = touchNormal;
}

void initBronzeDoor(Entity *e)
{
	initDoor(e, "gfx/entities/bronzeDoor.png");

	e->touch = touchBronze;
}

void initBlueDoor(Entity *e)
{
	initDoor(e, "gfx/entities/blueDoor.png");

	e->touch = touchBlue;
}

static void tick(Entity *self)
{
	Door *d;

	d = (Door *)self->data;

	if (d->open && self->y > d->openY)
	{
		self->y = MAX(self->y - 3 * app.deltaTime, d->openY);
	}
}

static void draw(Entity *self)
{
	blitAtlasImage(self->texture, self->x - stage.camera.x, self->y - stage.camera.y, 0, SDL_FLIP_NONE);
}

static void touchNormal(Entity *self, Entity *other)
{
	Door *d;

	if (other == stage.player)
	{
		d = (Door *)self->data;

		if (!d->open)
		{
			d->open = 1;
		}
	}
}

static void touchBronze(Entity *self, Entity *other)
{
	Door *d;

	if (other == stage.player && stage.bronzeKeys > 0)
	{
		d = (Door *)self->data;

		if (!d->open)
		{
			d->open = 1;

			stage.bronzeKeys--;
		}
	}
}

static void touchBlue(Entity *self, Entity *other)
{
	Door *d;

	if (other == stage.player && stage.blueKeys > 0)
	{
		d = (Door *)self->data;

		if (!d->open)
		{
			d->open = 1;

			stage.blueKeys--;
		}
	}
}

static void postLoad(Entity *self)
{
	Door *d;

	d = (Door *)self->data;

	d->openY = self->y - (self->h - 8);
}
