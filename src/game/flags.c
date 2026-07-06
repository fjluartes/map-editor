/*
 * Copyright (C) 2022 Parallel Realities. All rights reserved.
 */

#include "../common.h"

#include "../system/atlas.h"
#include "../system/draw.h"
#include "flags.h"

extern Stage stage;

static void draw(Entity *self);
static void endTouch(Entity *self, Entity *other);

static void initFlag(Entity *e, char *filename)
{
	e->texture = getAtlasImage(filename, 1);
	e->w = e->texture->rect.w;
	e->h = e->texture->rect.h;
	e->editorFlags = EMF_UNIQUE + EMF_REQUIRED;

	e->draw = draw;
}

void initStartFlag(Entity *e)
{
	STRCPY(e->name, "Start Flag");

	initFlag(e, "gfx/entities/startFlag.png");
}

void initEndFlag(Entity *e)
{
	STRCPY(e->name, "End Flag");

	initFlag(e, "gfx/entities/endFlag.png");

	e->touch = endTouch;
}

static void draw(Entity *self)
{
	blitAtlasImage(self->texture, self->x - stage.camera.x, self->y - stage.camera.y, 0, SDL_FLIP_NONE);
}

static void endTouch(Entity *self, Entity *other)
{
	if (other == stage.player && stage.numLollipops == stage.totalLollipops)
	{
		exit(1);
	}
}
