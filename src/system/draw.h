/*
 * Copyright (C) 2022 Parallel Realities. All rights reserved.
 */

void initDraw(void);
void prepareScene(void);
void presentScene(void);
void blit(SDL_Texture *texture, int x, int y, int center);
void blitAtlasImage(AtlasImage *atlasImage, int x, int y, int center, SDL_RendererFlip flip);
void blitRotated(AtlasImage *atlasImage, int x, int y, double angle);
void blitScaled(AtlasImage *atlasImage, int x, int y, int w, int h, int center);
void drawRect(int x, int y, int w, int h, int r, int g, int b, int a);
void drawOutlineRect(int x, int y, int w, int h, int r, int g, int b, int a);
void drawGridLines(int spacing);
