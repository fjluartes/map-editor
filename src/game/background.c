/*
 * Copyright (C) 2022 Parallel Realities. All rights reserved.
 */

#include "../common.h"

#include "../system/atlas.h"
#include "../system/draw.h"

static AtlasImage *backgroundTexture;

void initBackground(void)
{
	backgroundTexture = getAtlasImage("gfx/backgrounds/mountains.png", 1);
}

void drawBackground(void)
{
	blitScaled(backgroundTexture, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
}
