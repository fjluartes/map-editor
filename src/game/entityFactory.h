/*
 * Copyright (C) 2021,2022 Parallel Realities. All rights reserved.
 */

void     initEntityFactory(void);
Entity  *initEntity(char *name);
Entity **getEditorEntities(int *total);
Entity  *spawnEntity(void);
