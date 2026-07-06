/*
 * Copyright (C) 2021,2022 Parallel Realities. All rights reserved.
 */

#include "../common.h"

#include "doors.h"
#include "entityFactory.h"
#include "flags.h"
#include "keys.h"
#include "lollipop.h"
#include "player.h"
#include "spikes.h"

static InitFunc *getInitFunc(char *name);
static void      addInitFunc(char *name, void (*init)(Entity *e));

extern Stage stage;

static InitFunc head, *tail;

void initEntityFactory(void)
{
	memset(&head, 0, sizeof(InitFunc));
	tail = &head;

	addInitFunc("player", initPlayer);
	addInitFunc("floorSpikes", initFloorSpikes);
	addInitFunc("roofSpikes", initRoofSpikes);
	addInitFunc("leftSpikes", initLeftSpikes);
	addInitFunc("rightSpikes", initRightSpikes);
	addInitFunc("redLollipop", initRedLollipop);
	addInitFunc("yellowLollipop", initYellowLollipop);
	addInitFunc("greenLollipop", initGreenLollipop);
	addInitFunc("blueLollipop", initBlueLollipop);
	addInitFunc("purpleLollipop", initPurpleLollipop);
	addInitFunc("randomLollipop", initRandomLollipop);
	addInitFunc("startFlag", initStartFlag);
	addInitFunc("endFlag", initEndFlag);
	addInitFunc("bronzeKey", initBronzeKey);
	addInitFunc("blueKey", initBlueKey);
	addInitFunc("door", initNormalDoor);
	addInitFunc("bronzeDoor", initBronzeDoor);
	addInitFunc("blueDoor", initBlueDoor);
}

Entity *initEntity(char *name)
{
	Entity *e;

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Creating entity '%s'", name);

	e = spawnEntity();

	getInitFunc(name)->init(e);

	STRCPY(e->typeName, name);

	return e;
}

Entity **getEditorEntities(int *total)
{
	InitFunc *i;
	Entity   *e, **ents;
	int       num;

	num = 0;

	for (i = head.next; i != NULL; i = i->next)
	{
		num += 1;
	}

	ents = malloc(sizeof(Entity *) * num);

	num = 0;

	for (i = head.next; i != NULL; i = i->next)
	{
		e = malloc(sizeof(Entity));
		memset(e, 0, sizeof(Entity));
		i->init(e);

		STRCPY(e->typeName, i->name);

		ents[num++] = e;
	}

	*total = num;

	return ents;
}

Entity *spawnEntity(void)
{
	Entity *e;

	e = malloc(sizeof(Entity));
	memset(e, 0, sizeof(Entity));
	stage.entityTail->next = e;
	stage.entityTail = e;

	return e;
}

static void addInitFunc(char *name, void (*init)(Entity *e))
{
	InitFunc *i;

	i = malloc(sizeof(InitFunc));
	memset(i, 0, sizeof(InitFunc));
	tail->next = i;
	tail = i;

	STRCPY(i->name, name);
	i->init = init;
}

static InitFunc *getInitFunc(char *name)
{
	InitFunc *i;

	for (i = head.next; i != NULL; i = i->next)
	{
		if (strcmp(i->name, name) == 0)
		{
			return i;
		}
	}

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_CRITICAL, "Unknown entity '%s'", name);
	exit(1);

	return NULL;
}
