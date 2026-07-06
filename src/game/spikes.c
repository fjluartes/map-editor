/*
 * Copyright (C) 2022 Parallel Realities. All rights reserved.
 */

#include "../common.h"

#include "../system/atlas.h"
#include "../system/draw.h"
#include "spikes.h"

extern Stage stage;

static void draw(Entity *self);
static void touch(Entity *self, Entity *other);

static void initSpikes(Entity *e, char *texture, int weightless)
{
	e->texture = getAtlasImage(texture, 1);
	e->w = e->texture->rect.w;
	e->h = e->texture->rect.h;
	e->flags = EF_SOLID;

	if (weightless)
	{
		e->flags += EF_WEIGHTLESS;
	}

	e->draw = draw;
	e->touch = touch;
}

void initFloorSpikes(Entity *e)
{
	initSpikes(e, "gfx/entities/spikes.png", 0);
}

void initRoofSpikes(Entity *e)
{
	initSpikes(e, "gfx/entities/roofSpikes.png", 1);
}

void initLeftSpikes(Entity *e)
{
	initSpikes(e, "gfx/entities/leftSpikes.png", 1);
}

void initRightSpikes(Entity *e)
{
	initSpikes(e, "gfx/entities/rightSpikes.png", 1);
}

static void draw(Entity *self)
{
	blitAtlasImage(self->texture, self->x - stage.camera.x, self->y - stage.camera.y, 0, SDL_FLIP_NONE);
}

static void touch(Entity *self, Entity *other)
{
	if (other == stage.player)
	{
		if (self->flags & EF_WEIGHTLESS)
		{
			other->dead = 1;
		}
		else if (other->riding == self)
		{
			other->dead = 1;
		}
	}
}
