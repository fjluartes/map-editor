/*
 * Copyright (C) 2022 Parallel Realities. All rights reserved.
 */

#include <SDL_mixer.h>

#include "../common.h"

#include "draw.h"

extern App app;

void initDraw(void)
{
}

void prepareScene(void)
{
	SDL_SetRenderDrawColor(app.renderer, 32, 64, 96, 255);

	SDL_RenderClear(app.renderer);
}

void presentScene(void)
{
	SDL_RenderPresent(app.renderer);
}

void blit(SDL_Texture *texture, int x, int y, int center)
{
	SDL_Rect dest;

	dest.x = x;
	dest.y = y;
	SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);

	if (center)
	{
		dest.x -= dest.w / 2;
		dest.y -= dest.h / 2;
	}

	SDL_RenderCopy(app.renderer, texture, NULL, &dest);
}

void blitAtlasImage(AtlasImage *atlasImage, int x, int y, int center, SDL_RendererFlip flip)
{
	SDL_Rect dest;

	dest.x = x;
	dest.y = y;
	dest.w = atlasImage->rect.w;
	dest.h = atlasImage->rect.h;

	if (center)
	{
		dest.x -= dest.w / 2;
		dest.y -= dest.h / 2;
	}

	SDL_RenderCopyEx(app.renderer, atlasImage->texture, &atlasImage->rect, &dest, 0, NULL, flip);
}

void blitRotated(AtlasImage *atlasImage, int x, int y, double angle)
{
	SDL_Rect dstRect;

	dstRect.x = x;
	dstRect.y = y;
	dstRect.w = atlasImage->rect.w;
	dstRect.h = atlasImage->rect.h;

	dstRect.x -= (dstRect.w / 2);
	dstRect.y -= (dstRect.h / 2);

	SDL_RenderCopyEx(app.renderer, atlasImage->texture, &atlasImage->rect, &dstRect, angle, NULL, SDL_FLIP_NONE);
}

void blitScaled(AtlasImage *atlasImage, int x, int y, int w, int h, int center)
{
	SDL_Rect dstRect;

	dstRect.x = x;
	dstRect.y = y;
	dstRect.w = w;
	dstRect.h = h;

	if (center)
	{
		dstRect.x -= (dstRect.w / 2);
		dstRect.y -= (dstRect.h / 2);
	}

	SDL_RenderCopy(app.renderer, atlasImage->texture, &atlasImage->rect, &dstRect);
}

void drawRect(int x, int y, int w, int h, int r, int g, int b, int a)
{
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;

	SDL_SetRenderDrawBlendMode(app.renderer, a < 255 ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE);
	SDL_SetRenderDrawColor(app.renderer, r, g, b, a);
	SDL_RenderFillRect(app.renderer, &rect);
}

void drawOutlineRect(int x, int y, int w, int h, int r, int g, int b, int a)
{
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;

	SDL_SetRenderDrawBlendMode(app.renderer, a < 255 ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE);
	SDL_SetRenderDrawColor(app.renderer, r, g, b, a);
	SDL_RenderDrawRect(app.renderer, &rect);
}

void drawGridLines(int spacing)
{
	int x, y;

	SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 255);

	for (x = 0; x < SCREEN_WIDTH; x += spacing)
	{
		SDL_RenderDrawLine(app.renderer, x, 0, x, SCREEN_HEIGHT);
	}

	for (y = 0; y < SCREEN_WIDTH; y += spacing)
	{
		SDL_RenderDrawLine(app.renderer, 0, y, SCREEN_WIDTH, y);
	}
}
