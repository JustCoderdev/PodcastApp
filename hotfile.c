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
void ntos(n32 number, n32 len, const char* string) {
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

void draw_list(Rectangle bbox, HGL_State* state, Palette palette)
{
	Vector2 mouse = GetMousePosition();

	n8 visible_events_count;
	Rectangle event_bbox = {0};
	event_bbox.x = bbox.x + PAD;
	event_bbox.width = bbox.width - BAR_WIDTH - DPAD - HPAD;
	event_bbox.height = FONT_SIZE * 2 + PAD;
	visible_events_count = (bbox.height - DPAD) / (event_bbox.height + PAD);


	/* Handle input */
	{
		bool hovering = CollisionPointRec(mouse, bbox);
		i8 wheel = (i8)GetMouseWheelMove();
		assert(wheel == -1 || wheel == 0 || wheel == 1);

		if(hovering && wheel != 0)
		{
			state->offset -= wheel;

			/* Down v */
			if(wheel == -1) {
				if(state->offset + visible_events_count >= state->events.count)
					state->offset = state->events.count - visible_events_count - 1;
			}

 			/* Up ^ */
			if(wheel == 1) {
				/* If offset overflows... */
				if(state->offset >= state->events.count)
					state->offset = 0;
			}
		}

		if(IsKeyPressed(KEY_DOWN)) {
			state->cevent_index++;
			if(state->cevent_index >= state->events.count)
				state->cevent_index = 0;

			state->offset = clamp(0,
					(state->cevent_index / visible_events_count) * visible_events_count,
					state->events.count - visible_events_count);
		}

		if(IsKeyPressed(KEY_UP)) {
			state->cevent_index--;
			if(state->cevent_index > state->events.count)
				state->cevent_index = state->events.count - 1;

			state->offset = clamp(0,
					(state->cevent_index / visible_events_count) * visible_events_count,
					state->events.count - visible_events_count);
		}
	}


	/* */
	{
		String* buffer = &state->buffer;

		n16 i;
		for(i = 0; i < visible_events_count + 1; ++i) {
			n32 event_id = (i + state->offset) % state->events.count;
			MEvent event = state->events.items[event_id];

			Color fcolor = palette.fcolor;
			Color bcolor = palette.bcolor;

			bool hovering;
			bool selected;

			event_bbox.y = PAD + (event_bbox.height + PAD) * i;

			/* Handle inputs */
			hovering = CollisionPointRec(mouse, event_bbox);
			if(hovering && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
				state->cevent_index = event_id;
			}
			selected = event_id <= state->cevent_index;


			/* Draw background */
			if(hovering) { fcolor = RED; bcolor = DARK_RED; }
			if(selected) cinv(&fcolor, &bcolor);
			DrawRectangleRec(event_bbox, bcolor);
			DrawRectangleLinesEx(event_bbox, HBORD, selected ? bcolor : fcolor);


			/* */
			string_clear(buffer);

			switch(event.type) {
				case MALLOC:
					/* #1 MALLOC 64B */
					string_fmt(buffer, "#%u %s %dB", event_id,
							EVENT_TYPE_LABEL[event.type], event.as.malloc.size);
					break;

				case REALLOC:
					/* #1 REALLOC 0xABC 64B */
					string_fmt(buffer, "#%u %s 0x%x %dB", event_id,
							EVENT_TYPE_LABEL[event.type], event.as.realloc.fptr,
							event.as.realloc.size);
					break;

				case CALLOC:
					/* #1 CALLOC 1 64b */
					string_fmt(buffer, "#%u %s %d %dB", event_id,
							EVENT_TYPE_LABEL[event.type], event.as.calloc.memb,
							event.as.calloc.size);
					break;
					/* 0xABCDEF0123 */

				case FREE:
					/* #1 FREE 0xABC */
					string_fmt(buffer, "#%u %s 0x%x", event_id,
							EVENT_TYPE_LABEL[event.type], event.as.free.ptr);
					break;

				default:
					printf("event.type = %d\n", event.type);
					assert(0);
			}

			DrawText(buffer->chars, event_bbox.x + HPAD, event_bbox.y + HPAD, FONT_SIZE, fcolor);

			if(hovering) {
				DrawText(event.file_line.chars, event_bbox.x + HPAD, event_bbox.y + FONT_SIZE + HPAD, FONT_SIZE, fcolor);
			} else {
				string_clear(buffer);

				switch(event.type) {
					case MALLOC:
						string_fmt(buffer, "0x%x", event.as.malloc.rptr);
						break;

					case REALLOC:
						string_fmt(buffer, "0x%x", event.as.realloc.rptr);
						break;

					case CALLOC:
						string_fmt(buffer, "0x%x", event.as.calloc.rptr);
						break;

					case FREE:
						string_fmt(buffer, "I'm FREEEEE");
						break;

					default: assert(0);
				}

				DrawText(buffer->chars, event_bbox.x + HPAD, event_bbox.y + FONT_SIZE + HPAD, FONT_SIZE, fcolor);
			}
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
		draw_bar(box, &state->offset, state->events.count,
				visible_events_count, palette);

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
	draw_list(bbox, state, palette);

	/* Draw event view */
	bbox.x = 0;
	bbox.y = FONT_SIZE + PAD + HPAD;
	bbox.width = WINDOW_WIDTH - bbox.width + BORD;
	bbox.height = WINDOW_HEIGHT - bbox.y;
	draw_mainview(bbox, state, palette);

	/* Extern border */
	bbox.width = WINDOW_WIDTH;
	bbox.height = WINDOW_HEIGHT;
	bbox.x = 0;
	bbox.y = 0;
	DrawRectangleLinesEx(bbox, BORD, palette.fcolor);


#if DEBUG_ENABLE
	ntos(fps_string, FPS_STR_LEN, (n64)GetFPS());
	DrawText(fps_string, PAD, PAD, FONT_SIZE, CLR_DEBUG);

	ntos(tick_string, TICK_STR_LEN, state->tick);
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

