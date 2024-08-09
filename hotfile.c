#include <hotdraw.h>

#include <core.h>

#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <raylib.h>


#define HPAD 5
#define PAD  10
#define DPAD 20
#define QPAD 40

#define FONT_SIZE 20

#define HBORD 2
#define BORD  4
#define DBORD 8
#define QBORD 16

#define BAR_WIDTH (DPAD)

#define REC_FMT "x %0.1f, y %0.1f, width %0.1f, height %0.1f"
#define REC(RECT) (RECT).x, (RECT).y, (RECT).width, (RECT).height

/* const Color DARK_RED = {  94,  57,  60, 255 }; */
const Color DARK_RED = {  53,  20,  24, 255 };
const Color RED      = { 230,  41,  55, 255 };
const Color WHITE    = { 255, 255, 255, 255 };

Color htoc(n32 hex) { /* 0x RR GG BB AA */
	Color fcolor = {0};
	fcolor.r = hex >> 24;
	fcolor.g = hex >> 16;
	fcolor.b = hex >> 8;
	fcolor.a = hex;
	return fcolor;
}

Color GetRandomColor(n32 seed) {
	SetRandomSeed(seed);
	Color color = {0};
	float heat = GetRandomValue(0, 99);
	color.r = GetRandomValue(20 + 5 * (heat / 3    ), 255);
	color.g = GetRandomValue(20 + 5 * (heat / 3 * 2), 255);
	color.b = GetRandomValue(20 + 5 * (heat        ), 255);
	color.a = 255;
	return color;
}

bool CollisionPointRec(Vector2 p, Rectangle r) {
	return p.x > r.x && p.x < r.x + r.width
		&& p.y > r.y && p.y < r.y + r.height;
}

const Color CLR_FOREG = {  24, 195, 124, 255 };
const Color CLR_DEBUG = {  62,  62,  62, 255 };
const Color CLR_BACKG = {  30,  30,  30, 255 };

#define TICK_STR_LEN (20 + 1)
#define FPS_STR_LEN (2 + 1)

char tick_string[TICK_STR_LEN] = {0};
char fps_string[FPS_STR_LEN] = {0};
void ntos(n32 number, n32 len, char* string) {
	n64 i;

	for(i = 0; i < len - 1; ++i) {
		if(number == 0) {
			string[len - i - 2] = '-';
			continue;
		}

		{
			n8 rest = number % 10;
			string[len - i - 2] = '0' + rest;
			number -= rest;
		}

		if(number != 0) number /= 10;
	}

	string[len - 1] = '\0';
}

void init_fn(HGL_State* state) {
	int i;

	state->palette.fcolor = CLR_FOREG;
	state->palette.bcolor = CLR_BACKG;

	for(i = 0; i < 255; ++i)
		arr_append(&state->events, i);

	printf("Loaded module!\n");
}


void draw_bar(Rectangle bbox, n64* progress, n64 total, n64 visible_items, Palette palette) {
	Rectangle cursor = {0};
	float seg = bbox.height / total;

	cursor.width = bbox.width;
	cursor.height = seg * visible_items;
	cursor.x = bbox.x;

	{
		/* Handle input */
		Vector2 mouse = GetMousePosition();
		bool hovering = CollisionPointRec(mouse, bbox);
		bool dragging = hovering && IsMouseButtonDown(MOUSE_BUTTON_LEFT);
		/* bool shifting = IsKeyPressed(KEY_LEFT_SHIFT) || IsKeyPressed(KEY_RIGHT_SHIFT); */

		SetMouseCursor(dragging ? MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT);
		if(dragging) {
			float hcursor = cursor.height / 2;
			float mouseprog = clamp(bbox.y, mouse.y, bbox.y + bbox.height - hcursor);
			*progress = max(0, mouseprog - hcursor) / seg;
		}
	}

	cursor.y = bbox.y + (*progress * seg);

	DrawRectangleLinesEx(bbox, BORD, palette.fcolor);
	DrawRectangleRec(cursor, palette.fcolor);
}

void swap(void* A, void* B) {
	void* T = A;
	A = B;
	B = A;
}

typedef struct ViewState {
	int hovering: 1;
	int selected: 1;
} ViewState;

typedef void (*DrawElementFunc)(Rectangle bbox, void* elements, n64 element_id, ViewState state, Palette palette);
void draw_int(Rectangle bbox, void* elements, n64 element_id,
              ViewState state, Palette palette)
{
	Color fcolor = palette.fcolor;
	Color bcolor = palette.bcolor;

	/* Draw background */
	if(state.hovering) { fcolor = RED; bcolor = DARK_RED; }
	if(state.selected) swap(&fcolor, &bcolor);
	DrawRectangleRec(bbox, bcolor);
	DrawRectangleLinesEx(bbox, HBORD, state.selected ? bcolor : fcolor);

	/* */
	{
		String buffer = {0};
		int number = ((int*)elements)[element_id];
		string_fmt(&buffer, "%d", number);
		string_nterm(&buffer);

		DrawText(buffer.chars, bbox.x + bbox.height / 2 - FONT_SIZE/2,
				bbox.y + PAD, FONT_SIZE, fcolor);

		string_free(&buffer);
	}
}

void draw_list(Rectangle bbox, n64* offset, void* elements, n64 elements_count,
               DrawElementFunc draw_element_func, Palette palette)
{
	Vector2 mouse = GetMousePosition();

	n8 visible_elements_count;
	Rectangle element_bbox = {0};
	element_bbox.x = bbox.x + PAD;
	element_bbox.width = bbox.width - BAR_WIDTH - DPAD - HPAD;
	element_bbox.height = FONT_SIZE * 2 + PAD;
	visible_elements_count = (bbox.height - DPAD) / (element_bbox.height + PAD);


	/* Handle input */
	{
		bool hovering = CollisionPointRec(mouse, bbox);
		i8 wheel = (i8)GetMouseWheelMove();
		assert(wheel == -1 || wheel == 0 || wheel == 1);

		if(hovering && wheel != 0)
		{
			*offset -= wheel;

			/* Down v */
			if(wheel == -1) {
				if(*offset + visible_elements_count >= elements_count)
					*offset = elements_count - visible_elements_count - 1;
			}

 			/* Up ^ */
			if(wheel == 1) {
				/* If offset overflows... */
				if(*offset >= elements_count) *offset = 0;
			}
		}

		/* if(IsKeyPressed(KEY_DOWN)) { */
		/* 	state->cevent_index++; */
		/* 	if(state->cevent_index >= elements.count) */
		/* 		state->cevent_index = 0; */

		/* 	state->offset = clamp(0, */
		/* 			(state->cevent_index / visible_elements_count) * visible_elements_count, */
		/* 			state->elements.count - visible_elements_count); */
		/* } */

		/* if(IsKeyPressed(KEY_UP)) { */
		/* 	state->cevent_index--; */
		/* 	if(state->cevent_index > state->elements.count) */
		/* 		state->cevent_index = state->elements.count - 1; */

		/* 	state->offset = clamp(0, */
		/* 			(state->cevent_index / visible_elements_count) * visible_elements_count, */
		/* 			state->elements.count - visible_elements_count); */
		/* } */
	}


	/* */
	{
		n16 i;
		for(i = 0; i < visible_elements_count + 1; ++i) {
			n32 element_id = (i + *offset) % elements_count;
			ViewState state = {0};

			element_bbox.y = PAD + (element_bbox.height + PAD) * i;

			/* Handle inputs */
			state.hovering = CollisionPointRec(mouse, bbox);
			/* if(state.hovering && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) { */
			/* 	state->cevent_index = element_id; */
			/* } */
			/* state.selected = element_id <= state->cevent_index; */

			(*draw_element_func)(element_bbox, elements, element_id, state, palette);
		}
	}


	/* Draw Border */
	{
		Rectangle box = {0};

		/* Scroll bar */
		box.x = bbox.x + bbox.width - BAR_WIDTH - PAD;
		box.y = PAD;
		box.width = BAR_WIDTH;
		box.height = bbox.height - DPAD;
		draw_bar(box, offset, elements_count,
				visible_elements_count, palette);

		/* Clip content */
		box.x = bbox.x;
		box.y = bbox.y;
		box.width = bbox.width;
		box.height = bbox.height;
		DrawRectangleLinesEx(box, PAD, palette.bcolor);

		/* Event queue border */
		DrawRectangleLinesEx(bbox, BORD, palette.fcolor);
	}
}


void tick_fn(HGL_State* state) {
	const n16 WINDOW_WIDTH = GetScreenWidth();
	const n16 WINDOW_HEIGHT = GetScreenHeight();

	const Palette palette = state->palette;
	Rectangle bbox;

	BeginDrawing();
	ClearBackground(palette.bcolor);

	/* Draw sidebar */
	bbox.width = 350 + BAR_WIDTH + DPAD;
	bbox.height = WINDOW_HEIGHT;
	bbox.x = WINDOW_WIDTH - bbox.width;
	bbox.y = 0;
	draw_list(bbox, &state->offset, &state->events.items,
			state->events.count, draw_int, palette);


	/* Extern border */
	bbox.width = WINDOW_WIDTH;
	bbox.height = WINDOW_HEIGHT;
	bbox.x = 0;
	bbox.y = 0;
	DrawRectangleLinesEx(bbox, BORD, palette.fcolor);


#if DEBUG_ENABLE
	ntos(GetFPS(), FPS_STR_LEN, fps_string);
	DrawText(fps_string, PAD, PAD, FONT_SIZE, CLR_DEBUG);

	ntos(state->tick, TICK_STR_LEN, tick_string);
	{
		n8 text_width = MeasureText("2", FONT_SIZE) * strlen(tick_string);
		DrawText(tick_string, WINDOW_WIDTH - 390 - DPAD - text_width,
				PAD, FONT_SIZE, CLR_DEBUG);
		state->tick++;
	}

	DrawCircleV(GetMousePosition(), 5, RED);
#endif

	EndDrawing();
}

void stop_fn(HGL_State* state) {
	(void)state;
}

