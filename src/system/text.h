/*
 * Copyright (C) 2022 Parallel Realities. All rights reserved.
 */

void         initFonts(void);
SDL_Texture *getTextTexture(char *text, int type);
void         drawText(char *text, int x, int y, int r, int g, int b, int align, int maxWidth);
void         calcTextDimensions(char *text, int *w, int *h);
int          getWrappedTextHeight(char *text, int maxWidth);
