/*
 * Copyright (C) 2022 Parallel Realities. All rights reserved.
 */

unsigned long hashcode(const char *str);
int           collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
int           getDistance(int x1, int y1, int x2, int y2);
double        getAngle(int x1, int y1, int x2, int y2);
void          calcSlope(int x1, int y1, int x2, int y2, double *dx, double *dy);
int           fileExists(const char *filename);
char		 *readFile(char *filename);
int           writeFile(const char *filename, const char *data);
char		 *formattedString(const char *format, ...);
