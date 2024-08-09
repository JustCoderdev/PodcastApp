#ifndef HGL_HOTDRAW_
#define HGL_HOTDRAW_

#include <core.h>
#include <raylib.h>

typedef struct Palette {
	Color fcolor;
	Color bcolor;
} Palette;

typedef struct Ints {
	int* items;
	n64 count;
	n64 capacity;
} Ints;

typedef struct {
	Palette palette;
	n64 tick;
	n64 offset;
	Ints events;
} HGL_State;


extern error HGL_load(char* path);
extern error HGL_reload(void);
extern error HGL_unload(void);

extern void (*HGL_init_fn)(HGL_State* state);
extern void (*HGL_tick_fn)(HGL_State* state);
extern void (*HGL_stop_fn)(HGL_State* state);

#endif /* HGL_HOTDRAW_ */
