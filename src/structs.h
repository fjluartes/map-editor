/*
 * Copyright (C) 2022 Parallel Realities. All rights reserved.
 */

typedef struct AtlasImage AtlasImage;
typedef struct Texture    Texture;
typedef struct Entity     Entity;
typedef struct InitFunc   InitFunc;
typedef struct Effect     Effect;

struct AtlasImage
{
	char         filename[MAX_FILENAME_LENGTH];
	SDL_Texture *texture;
	SDL_Rect     rect;
	int          rotated;
	AtlasImage  *next;
};

struct Texture
{
	char         name[MAX_FILENAME_LENGTH];
	SDL_Texture *texture;
	Texture     *next;
};

struct InitFunc
{
	char name[MAX_NAME_LENGTH];
	void (*init)(Entity *e);
	InitFunc *next;
};

struct Entity
{
	char          name[MAX_NAME_LENGTH];
	char          typeName[MAX_NAME_LENGTH];
	double        x;
	double        y;
	int           w;
	int           h;
	int           dead;
	double        dx;
	double        dy;
	int           isOnGround;
	int           layer;
	unsigned long flags;
	unsigned long editorFlags;
	AtlasImage   *texture;
	Entity       *riding;
	Entity       *next;
	void(*data);
	void (*tick)(Entity *self);
	void (*draw)(Entity *self);
	void (*touch)(Entity *self, Entity *other);
	void (*postLoad)(Entity *self);
	void (*die)(Entity *self);
};

typedef struct
{
	int canDoubleJump;
} Slime;

typedef struct
{
	double bobValue;
} Item;

typedef struct
{
	int open;
	int openY;
} Door;

struct Effect
{
	double      x;
	double      y;
	double      dx;
	double      dy;
	double      life;
	double      alpha;
	int         weightless;
	AtlasImage *texture;
	Effect     *next;
};

typedef struct
{
	char    name[MAX_NAME_LENGTH];
	int     map[MAP_WIDTH][MAP_HEIGHT];
	Entity  entityHead, *entityTail;
	Entity  deadEntityHead, *deadEntityTail;
	Entity *player;
	Effect  effectHead, *effectTail;
	int     numLollipops, totalLollipops;
	int     bronzeKeys, blueKeys;
	struct
	{
		double x;
		double y;
	} camera;
} Stage;

typedef struct
{
	struct
	{
		void (*logic)(void);
		void (*draw)(void);
	} delegate;
	SDL_Renderer *renderer;
	SDL_Window   *window;
	int           keyboard[MAX_KEYBOARD_KEYS];
	double        deltaTime;
	double        fontScale;
	struct
	{
		int x;
		int y;
		int buttons[MAX_MOUSE_BUTTONS];
		int visible;
	} mouse;
	struct
	{
		int fps;
		int isEditor;
	} dev;
} App;
