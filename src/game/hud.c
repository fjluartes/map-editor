/*
 * Copyright (C) 2022 Parallel Realities. All rights reserved.
 */

#include "../common.h"

#include "../system/atlas.h"
#include "../system/draw.h"
#include "../system/text.h"
#include "hud.h"

extern App   app;
extern Stage stage;

static AtlasImage *lollipopIcon;
static AtlasImage *bronzeKeyIcon;
static AtlasImage *blueKeyIcon;

void initHud(void)
{
	lollipopIcon = getAtlasImage("gfx/entities/lollipop.png", 1);
	bronzeKeyIcon = getAtlasImage("gfx/entities/bronzeKey.png", 1);
	blueKeyIcon = getAtlasImage("gfx/entities/blueKey.png", 1);
}

void drawHud(void)
{
	char text[MAX_LINE_LENGTH];

	drawRect(0, 0, SCREEN_WIDTH, 48, 0, 0, 0, 192);

	blitAtlasImage(lollipopIcon, 15, 5, 0, SDL_FLIP_NONE);
	sprintf(text, "%d / %d", stage.numLollipops, stage.totalLollipops);
	drawText(text, 80, 0, 255, 255, 255, TEXT_ALIGN_LEFT, 0);

	blitRotated(bronzeKeyIcon, 335, 20, 90);
	sprintf(text, "%d", stage.bronzeKeys);
	drawText(text, 375, 0, 255, 255, 255, TEXT_ALIGN_LEFT, 0);

	blitRotated(blueKeyIcon, 535, 20, 90);
	sprintf(text, "%d", stage.blueKeys);
	drawText(text, 575, 0, 255, 255, 255, TEXT_ALIGN_LEFT, 0);

	sprintf(text, "Stage: %s", stage.name);
	drawText(text, 900, 0, 255, 255, 255, TEXT_ALIGN_LEFT, 0);

	sprintf(text, "FPS: %d", app.dev.fps);
	drawText(text, SCREEN_WIDTH - 10, 0, 255, 255, 255, TEXT_ALIGN_RIGHT, 0);
}
