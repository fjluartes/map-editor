/*
 * Copyright (C) 2022 Parallel Realities. All rights reserved.
 */

#include "common.h"

#include "editor/editor.h"
#include "game/stage.h"
#include "main.h"
#include "system/draw.h"
#include "system/init.h"
#include "system/input.h"

static void logic(void);
static void doFPS(void);
static void handleCommandLine(int argc, char *argv[]);

static int  fps;
static long nextFPS;

App   app;
Stage stage;

int main(int argc, char *argv[])
{
	long then;

	memset(&app, 0, sizeof(App));

	initSDL();

	initGameSystem();

	handleCommandLine(argc, argv);

	atexit(cleanup);

	nextFPS = SDL_GetTicks() + 1000;

	while (1)
	{
		then = SDL_GetTicks();

		prepareScene();

		doInput();

		logic();

		app.delegate.draw();

		presentScene();

		/* allow the CPU/GPU to breathe */
		SDL_Delay(1);

		app.deltaTime = LOGIC_RATE * (SDL_GetTicks() - then);

		doFPS();
	}

	return 0;
}

static void logic(void)
{
	double tmpDelta;

	/* don't exceed target logic rate */
	while (app.deltaTime > 1)
	{
		tmpDelta = app.deltaTime;

		app.deltaTime = 1;

		app.delegate.logic();

		app.deltaTime = (tmpDelta - 1);
	}

	app.delegate.logic();
}

static void doFPS(void)
{
	fps++;

	if (SDL_GetTicks() >= nextFPS)
	{
		app.dev.fps = fps;

		fps = 0;

		nextFPS = SDL_GetTicks() + 1000;
	}
}

static void handleCommandLine(int argc, char *argv[])
{
	int ok;

	ok = 0;

	if (argc >= 3)
	{
		memset(&stage, 0, sizeof(Stage));

		STRCPY(stage.name, argv[2]);

		if (strcmp(argv[1], "-edit") == 0)
		{
			initEditor();

			ok = 1;
		}
		else if (strcmp(argv[1], "-map") == 0)
		{
			initStage();

			ok = 1;
		}
	}

	if (!ok)
	{
		printf("Use -edit <name> or -map <name>.\n");

		exit(1);
	}
}
