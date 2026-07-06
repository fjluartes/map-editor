/*
 * Copyright (C) 2022 Parallel Realities. All rights reserved.
 */

#include "../common.h"

#include "../system/atlas.h"
#include "../system/draw.h"
#include "effects.h"

#define NUM_STAR_TEXTURES  6
#define NUM_SPLAT_TEXTURES 4

extern App   app;
extern Stage stage;

static AtlasImage *starTextures[NUM_STAR_TEXTURES];
static AtlasImage *splatTextures[NUM_SPLAT_TEXTURES];

void initEffects(void)
{
	int  i;
	char filename[MAX_NAME_LENGTH];

	memset(&stage.effectHead, 0, sizeof(Effect));
	stage.effectTail = &stage.effectHead;

	for (i = 0; i < NUM_STAR_TEXTURES; i++)
	{
		sprintf(filename, "gfx/effects/star%02d.png", i + 1);
		starTextures[i] = getAtlasImage(filename, 1);
	}

	for (i = 0; i < NUM_SPLAT_TEXTURES; i++)
	{
		sprintf(filename, "gfx/effects/splat%02d.png", i + 1);
		splatTextures[i] = getAtlasImage(filename, 1);
	}
}

void doEffects(void)
{
	Effect *e, *prev;

	prev = &stage.effectHead;

	for (e = stage.effectHead.next; e != NULL; e = e->next)
	{
		e->x += e->dx * app.deltaTime;
		e->y += e->dy * app.deltaTime;

		e->life -= app.deltaTime;

		if (!e->weightless)
		{
			e->dy += app.deltaTime;
		}

		if (e->weightless)
		{
			e->alpha -= 5 * app.deltaTime;
		}

		if (e->life <= 0 && e->alpha <= 0)
		{
			prev->next = e->next;

			if (e == stage.effectTail)
			{
				stage.effectTail = prev;
			}

			free(e);

			e = prev;
		}

		prev = e;
	}
}

void drawEffects(void)
{
	Effect *e;

	for (e = stage.effectHead.next; e != NULL; e = e->next)
	{
		SDL_SetTextureAlphaMod(e->texture->texture, e->alpha);

		blitAtlasImage(e->texture, e->x - stage.camera.x, e->y - stage.camera.y, 0, SDL_FLIP_NONE);
	}

	SDL_SetTextureAlphaMod(starTextures[0]->texture, 255);
}

void addStarEffects(int x, int y, int num)
{
	int     i;
	Effect *e;

	for (i = 0; i < num; i++)
	{
		e = malloc(sizeof(Effect));
		memset(e, 0, sizeof(Effect));
		stage.effectTail->next = e;
		stage.effectTail = e;

		e->x = x;
		e->y = y;
		e->dx = rand() % 250 - rand() % 250;
		e->dy = rand() % 250 - rand() % 250;
		e->life = (FPS / 4) + rand() % (FPS / 4);
		e->alpha = 192 + rand() % 64;
		e->texture = starTextures[rand() % NUM_STAR_TEXTURES];
		e->weightless = 1;

		e->dx *= 0.01;
		e->dy *= 0.01;
	}
}

void addSplatEffects(int x, int y, int num)
{
	int     i;
	Effect *e;

	for (i = 0; i < num; i++)
	{
		e = malloc(sizeof(Effect));
		memset(e, 0, sizeof(Effect));
		stage.effectTail->next = e;
		stage.effectTail = e;

		e->x = x;
		e->y = y;
		e->dx = rand() % 250 - rand() % 250;
		e->dy = -(12 + rand() % 12);
		e->life = (FPS / 2) + rand() % (FPS / 2);
		e->alpha = 255;
		e->texture = splatTextures[rand() % NUM_SPLAT_TEXTURES];

		e->dx *= 0.05;
	}
}
