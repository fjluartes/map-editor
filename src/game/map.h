/*
 * Copyright (C) 2022 Parallel Realities. All rights reserved.
 */

void initMap(void);
void drawMap(int layer);
int  isInsideMap(int x, int y);
int  isSolidMap(int x, int y);
int  isWater(int x, int y);
void loadMap(void);
int  saveMap(void);
