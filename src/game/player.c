/*
 * Copyright (C) 2022 Parallel Realities. All rights reserved.
 */

#include "../common.h"

#include "../system/atlas.h"
#include "../system/draw.h"
#include "effects.h"
#include "player.h"

#define MOVE_SPEED      8.0
#define JUMP_POWER      -10
#define NUM_ANIM_FRAMES 4

static void tick(Entity *self);
static void walk(Entity *self);
static void swim(void);
static void draw(Entity *self);
static void die(Entity *self);

extern App   app;
extern Stage stage;

static double      animTimer;
static int         frame;
static int         facing;
static AtlasImage *textures[NUM_ANIM_FRAMES];

void initPlayer(Entity *e)
{
	Slime *slime;

	slime = malloc(sizeof(Slime));
	memset(slime, 0, sizeof(Slime));

	textures[0] = getAtlasImage("gfx/entities/player01.png", 1);
	textures[1] = getAtlasImage("gfx/entities/player02.png", 1);
	textures[2] = getAtlasImage("gfx/entities/player03.png", 1);
	textures[3] = getAtlasImage("gfx/entities/player04.png", 1);

	animTimer = 0;
	frame = 0;
	facing = 0;

	e->texture = textures[frame];
	e->w = e->texture->rect.w;
	e->h = e->texture->rect.h;
	e->data = slime;
	e->editorFlags = EMF_UNIQUE + EMF_REQUIRED;

	e->tick = tick;
	e->draw = draw;
	e->die = die;

	stage.player = e;
}

static void tick(Entity *self)
{
	stage.player->dx = 0;

	if (stage.player->flags & EF_IN_WATER)
	{
		swim();
	}
	else
	{
		walk(self);
	}

	animTimer -= app.deltaTime;

	if (animTimer <= 0)
	{
		frame = (frame + 1) % NUM_ANIM_FRAMES;

		animTimer = 8;

		self->texture = textures[frame];
	}

	if (stage.player->dx != 0)
	{
		facing = stage.player->dx > 0 ? 0 : 1;
	}
}

static void swim(void)
{
	if (app.keyboard[SDL_SCANCODE_A])
	{
		stage.player->dx = -MOVE_SPEED * 0.5;
	}

	if (app.keyboard[SDL_SCANCODE_D])
	{
		stage.player->dx = MOVE_SPEED * 0.5;
	}

	if (app.keyboard[SDL_SCANCODE_I] || app.keyboard[SDL_SCANCODE_W])
	{
		stage.player->dy = -MOVE_SPEED * 0.5;
	}

	if (app.keyboard[SDL_SCANCODE_S])
	{
		stage.player->dy = MOVE_SPEED * 0.5;
	}
}

static void walk(Entity *self)
{
	Slime *slime;

	slime = (Slime *)self->data;

	if (stage.player->isOnGround)
	{
		slime->canDoubleJump = 1;
	}

	if (app.keyboard[SDL_SCANCODE_A])
	{
		stage.player->dx = -MOVE_SPEED;
	}

	if (app.keyboard[SDL_SCANCODE_D])
	{
		stage.player->dx = MOVE_SPEED;
	}

	if (app.keyboard[SDL_SCANCODE_I] && (stage.player->isOnGround || slime->canDoubleJump))
	{
		app.keyboard[SDL_SCANCODE_I] = 0;

		stage.player->riding = NULL;

		stage.player->dy = JUMP_POWER;

		if (!stage.player->isOnGround)
		{
			slime->canDoubleJump = 0;
		}
	}
}

static void draw(Entity *self)
{
	blitAtlasImage(self->texture, self->x - stage.camera.x, self->y - stage.camera.y, 0, facing == 0 ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL);
}

static void die(Entity *self)
{
	addSplatEffects(self->x + (self->w / 2), self->y + (self->h / 2), 12);
}
