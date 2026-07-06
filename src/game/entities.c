/*
 * Copyright (C) 2022 Parallel Realities. All rights reserved.
 */

#include "../common.h"

#include "../json/cJSON.h"
#include "../system/util.h"
#include "entities.h"
#include "entityFactory.h"
#include "map.h"

#define GRAVITY_POWER    0.5
#define WATER_SINK_SPEED 0.1

extern App   app;
extern Stage stage;

static void loadEntities(void);
static void move(Entity *e);
static void push(Entity *e, float dx, float dy);
static void moveToWorld(Entity *e, float dx, float dy);
static void moveToEntities(Entity *e, float dx, float dy);
static int  isInWater(Entity *e);

void initEntities(void)
{
	stage.entityTail = &stage.entityHead;
	stage.deadEntityTail = &stage.deadEntityHead;

	loadEntities();
}

void doEntities(void)
{
	Entity *e, *prev;

	prev = &stage.entityHead;

	for (e = stage.entityHead.next; e != NULL; e = e->next)
	{
		if (e->tick != NULL)
		{
			e->tick(e);
		}

		move(e);

		if (e->dead)
		{
			if (e->die != NULL)
			{
				e->die(e);
			}

			if (e == stage.entityTail)
			{
				stage.entityTail = prev;
			}

			prev->next = e->next;

			e->next = NULL;

			if (e != stage.player)
			{
				stage.deadEntityTail->next = e;
				stage.deadEntityTail = e;
			}

			e = prev;
		}

		prev = e;
	}

	for (e = stage.entityHead.next; e != NULL; e = e->next)
	{
		if (e->riding != NULL)
		{
			e->x += e->riding->dx * app.deltaTime;

			push(e, e->riding->dx, 0);
		}

		e->x = MIN(MAX(e->x, 0), MAP_WIDTH * MAP_TILE_SIZE);
		e->y = MIN(MAX(e->y, 0), MAP_HEIGHT * MAP_TILE_SIZE);
	}
}

static void move(Entity *e)
{
	int wasInWater, inWater;

	if (e->flags & EF_IN_WATER)
	{
		if (e->dy < 1)
		{
			e->dy += GRAVITY_POWER * app.deltaTime;
		}

		e->dy = MAX(MIN(e->dy, 4), -999);
	}
	else if (!(e->flags & EF_WEIGHTLESS))
	{
		e->dy += GRAVITY_POWER * app.deltaTime;
		e->dy = MAX(MIN(e->dy, 32), -999);
	}

	if (e->riding != NULL && e->riding->dy > 0)
	{
		e->dy = e->riding->dy + 1;
	}

	e->riding = NULL;

	e->isOnGround = 0;

	wasInWater = isInWater(e);

	e->x += e->dx * app.deltaTime;
	push(e, e->dx * app.deltaTime, 0);

	e->y += e->dy * app.deltaTime;
	push(e, 0, e->dy * app.deltaTime);

	inWater = isInWater(e);

	if (inWater != wasInWater)
	{
		if (inWater)
		{
			e->flags |= EF_IN_WATER;
			e->dy = 0;
		}
		else
		{
			e->flags &= ~EF_IN_WATER;
			e->dy = -8;
			e->isOnGround = 0;
		}
	}
}

static void push(Entity *e, float dx, float dy)
{
	moveToWorld(e, dx, dy);

	moveToEntities(e, dx, dy);
}

static void moveToWorld(Entity *e, float dx, float dy)
{
	int mx, my, hit, adj;

	if (dx != 0)
	{
		mx = dx > 0 ? (e->x + e->w) : e->x;
		mx /= MAP_TILE_SIZE;

		my = (e->y / MAP_TILE_SIZE);

		hit = 0;

		if (!isInsideMap(mx, my) || isSolidMap(mx, my))
		{
			hit = 1;
		}

		my = (e->y + e->h - 1) / MAP_TILE_SIZE;

		if (!isInsideMap(mx, my) || isSolidMap(mx, my))
		{
			hit = 1;
		}

		if (hit)
		{
			adj = dx > 0 ? -e->w : MAP_TILE_SIZE;

			e->x = (mx * MAP_TILE_SIZE) + adj;

			e->dx = 0;
		}
	}

	if (dy != 0)
	{
		my = dy > 0 ? (e->y + e->h) : e->y;
		my /= MAP_TILE_SIZE;

		mx = e->x / MAP_TILE_SIZE;

		hit = 0;

		if (!isInsideMap(mx, my) || isSolidMap(mx, my))
		{
			hit = 1;
		}

		mx = (e->x + e->w - 1) / MAP_TILE_SIZE;

		if (!isInsideMap(mx, my) || isSolidMap(mx, my))
		{
			hit = 1;
		}

		if (hit)
		{
			adj = dy > 0 ? -e->h : MAP_TILE_SIZE;

			e->y = (my * MAP_TILE_SIZE) + adj;

			e->dy = 0;

			e->isOnGround = dy > 0;
		}
	}
}

static void moveToEntities(Entity *e, float dx, float dy)
{
	Entity *other;
	int     adj;

	for (other = stage.entityHead.next; other != NULL; other = other->next)
	{
		if (other != e && collision(e->x, e->y, e->w, e->h, other->x, other->y, other->w, other->h))
		{
			if (other->flags & EF_SOLID)
			{
				if (dy != 0)
				{
					adj = dy > 0 ? -e->h : other->h;

					e->y = other->y + adj;

					e->dy = 0;

					if (dy > 0)
					{
						e->isOnGround = 1;

						e->riding = other;
					}
				}

				if (dx != 0)
				{
					adj = dx > 0 ? -e->w : other->w;

					e->x = other->x + adj;

					e->dx = 0;
				}
			}
			else if (e->flags & EF_PUSH)
			{
				other->x += e->dx * app.deltaTime;
				push(other, e->dx * app.deltaTime, 0);

				other->y += e->dy * app.deltaTime;
				push(other, 0, e->dy * app.deltaTime);
			}

			if (e->touch != NULL)
			{
				e->touch(e, other);
			}

			if (other->flags & EF_SOLID && other->touch != NULL)
			{
				other->touch(other, e);
			}
		}
	}
}

static int isInWater(Entity *e)
{
	int x, y;

	x = (e->x + (e->w / 2)) / MAP_TILE_SIZE;
	y = (e->y + (e->h / 2)) / MAP_TILE_SIZE;

	return isWater(x, y);
}

void drawEntities(int layer)
{
	Entity *e;

	for (e = stage.entityHead.next; e != NULL; e = e->next)
	{
		if (e->layer == layer)
		{
			e->draw(e);
		}
	}
}

static char *getEntitiesFilename(void)
{
	return formattedString("data/maps/%s.json", stage.name);
}

static void loadEntities(void)
{
	cJSON  *root, *node;
	char   *text, *type, *filename;
	int     x, y;
	Entity *e;

	filename = getEntitiesFilename();

	if (fileExists(filename))
	{
		text = readFile(filename);

		root = cJSON_Parse(text);

		for (node = root->child; node != NULL; node = node->next)
		{
			type = cJSON_GetObjectItem(node, "type")->valuestring;
			x = cJSON_GetObjectItem(node, "x")->valueint;
			y = cJSON_GetObjectItem(node, "y")->valueint;

			e = initEntity(type);
			e->x = x;
			e->y = y;

			if (e->postLoad != NULL)
			{
				e->postLoad(e);
			}
		}

		cJSON_Delete(root);

		free(text);
	}
	else if (!app.dev.isEditor)
	{
		printf("Couldn't load '%s'\n", filename);
		free(filename);
		exit(1);
	}

	free(filename);
}

int saveEntities(void)
{
	Entity *e;
	cJSON  *root, *entityJSON;
	char   *filename, *out;
	int     rtn;

	root = cJSON_CreateArray();

	for (e = stage.entityHead.next; e != NULL; e = e->next)
	{
		entityJSON = cJSON_CreateObject();

		cJSON_AddStringToObject(entityJSON, "type", e->typeName);
		cJSON_AddNumberToObject(entityJSON, "x", e->x);
		cJSON_AddNumberToObject(entityJSON, "y", e->y);

		if (strlen(e->name) > 0)
		{
			cJSON_AddStringToObject(entityJSON, "name", e->name);
		}

		cJSON_AddItemToArray(root, entityJSON);
	}

	out = cJSON_Print(root);

	filename = getEntitiesFilename();

	rtn = writeFile(filename, out);

	cJSON_Delete(root);

	free(filename);

	free(out);

	return rtn;
}
