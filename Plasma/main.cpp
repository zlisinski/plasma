#include <cmath>
#include <assert.h>
#include "SDL.h"

const Uint32 SCREEN_WIDTH = 640;
const Uint32 SCREEN_HEIGHT = 480;
const Uint32 SCREEN_BPP = 32;
const Uint32 PALETTE_SIZE = 256;
const Uint32 PALETTE_COUNT = 2;
const Uint32 PLASMA_COUNT = 3;

// PlasmaFunc is a function pointer to a getPlasmaPixel*() function
typedef Uint8 (*PlasmaFunc) (Uint32 x, Uint32 y, Uint32 frame);

void populatePalette1(Uint32 palette[PALETTE_SIZE]);
void populatePalette2(Uint32 palette[PALETTE_SIZE]);

Uint8 getPlasmaPixel1(Uint32 x, Uint32 y, Uint32 frame);
Uint8 getPlasmaPixel2(Uint32 x, Uint32 y, Uint32 frame);
Uint8 getPlasmaPixel3(Uint32 x, Uint32 y, Uint32 frame);

void drawScreen(SDL_Surface *screen, Uint32 frame, Uint32 palette[PALETTE_SIZE], PlasmaFunc);
void setPixel(SDL_Surface *screen, Uint32 x, Uint32 y, Uint8 r, Uint8 g, Uint8 b);

// Combine RGB bytes into a RGBA integer
#define RGBToInt(r, g, b) ((((r & 0xFF) << 24) + ((g & 0xFF) << 16) + ((b & 0xFF) << 8)) & 0xFFFFFF00)
// Extract color bytes from a RGBA integer
#define RedFromInt(color) (color >> 24)
#define GreenFromInt(color) ((color & 0x00FF0000) >> 16)
#define BlueFromInt(color) ((color & 0x0000FF00) >> 8)
// Calculate the distance between two points
#define Distance(x1, y1, x2, y2) sqrt(double((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)))

int main(int argc, char* args[]) {
	PlasmaFunc plasmaFuncs[PLASMA_COUNT] = {getPlasmaPixel1, getPlasmaPixel2, getPlasmaPixel3};
	Uint32 curPlasma = 2;
	Uint32 palette[PALETTE_COUNT][PALETTE_SIZE] = {0};
	Uint32 curPalette = 1;
	Uint32 frame = 0;
	bool running = true;
	SDL_Surface *screen = NULL;
	SDL_Event event;

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		return 1;

	// Set the window caption
	SDL_WM_SetCaption("Plasma Test - Press 'z' to change palette, Press 'x' to change plasma", NULL);

	// Set the screen size
	screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE);
	if (screen == NULL) {
		SDL_Quit();
		return 1;
	}

	// Fill palettes
	populatePalette1(palette[0]);
	populatePalette2(palette[1]);

	// Main loop
	while (running) {
		drawScreen(screen, frame++, palette[curPalette], plasmaFuncs[curPlasma]);

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
				break;
			}

			if (event.type == SDL_KEYDOWN) {
				SDLKey keyPressed = event.key.keysym.sym;
				if (keyPressed == SDLK_z) {
					curPalette = (curPalette + 1) % PALETTE_COUNT;
				}
				else if (keyPressed == SDLK_x) {
					curPlasma = (curPlasma + 1) % PLASMA_COUNT;
				}
			}
		}
	}

	// Cleanup
	SDL_Quit();

	return 0;
}

// Rainbow palette
void populatePalette1(Uint32 palette[PALETTE_SIZE]) {
	Uint8 r, g, b;

	for (int i = 0; i < 256; i++) {
		r = Uint8(127.5 + 128 * sin(3.1415 * i / 128.0));
		g = Uint8(127.5 + 128 * cos(3.1415 * i / 128.0));
		b = Uint8(127.5 + 128 * sin(3.1415 * i / 128.0) * -1);
		palette[i] = RGBToInt(r, g, b);
	}
}

// Blue-Green palette
void populatePalette2(Uint32 palette[PALETTE_SIZE]) {
	Uint8 r, g, b;

	for (int i = 0; i < 256; i++) {
		r = 0;
		g = Uint8(127.5 + 128 * cos(3.1415 * i / 128.0));
		b = Uint8(127.5 + 128 * sin(3.1415 * i / 128.0));
		palette[i] = RGBToInt(r, g, b);
	}
}

Uint8 getPlasmaPixel1(Uint32 x, Uint32 y, Uint32 frame) {
	Uint32 color = (Uint32)(
		128.0 + (128.0 * sin(x / 16.0)) +
		128.0 + (128.0 * sin(y / 8.0)) +
		128.0 + (128.0 * sin((x + y) / 16.0)) +
		128.0 + (128.0 * sin(sqrt(double(x * x + y * y)) / 8.0))
	) / 4;

	assert(color < 256);

	return color;
}

Uint8 getPlasmaPixel2(Uint32 x, Uint32 y, Uint32 frame) {
	Uint32 color = (Uint32)(
		128.0 + (128.0 * sin(x / 16.0)) +
		128.0 + (128.0 * sin(y / 8.0)) +
		128.0 + (128.0 * sin((x + y) / 16.0)) +
		128.0 + (128.0 * sin(sqrt(double(x * x + y * y)) / 8.0)) +
		128.0 + (128.0 * sin(Distance(x + frame, SCREEN_WIDTH / 2.0, y + frame, SCREEN_HEIGHT / 2.0) / 8.0))
	) / 5;

	assert(color < 256);

	return color;
}

Uint8 getPlasmaPixel3(Uint32 x, Uint32 y, Uint32 frame) {
	Uint32 color = (Uint32)(
		128.0 + (128.0 * sin(Distance(x + frame, y, SCREEN_WIDTH / 4.0, SCREEN_HEIGHT / 4.0) / 12.0)) +
		128.0 + (128.0 * sin(Distance(x, y, SCREEN_WIDTH / 4.0 * 3.0, SCREEN_HEIGHT / 4.0 * 3.0) / 12.0)) +
		128.0 + (128.0 * sin(Distance(x, y + frame / 5, SCREEN_WIDTH / 4.0 * 3.0, SCREEN_HEIGHT / 4.0) / 12.0)) +
		128.0 + (128.0 * sin(Distance(x, y, SCREEN_WIDTH / 4.0, SCREEN_HEIGHT / 4.0 * 3.0) / 12.0))
	) / 4;

	assert(color < 256);

	return color;
}

void drawScreen(SDL_Surface *screen, Uint32 frame, Uint32 palette[PALETTE_SIZE], PlasmaFunc plasmaFunc) {
	Uint32 x, y;
	Uint32 paletteShift = frame;// / 2;
	Uint8 paletteIndex;
	Uint32 color;

	if (SDL_MUSTLOCK(screen)) {
		if (SDL_LockSurface(screen) < 0)
			return;
	}

	for (y = 0; y < SCREEN_HEIGHT; y++) {
		for (x = 0; x < SCREEN_WIDTH; x++) {
			paletteIndex = (plasmaFunc(x, y, frame) + paletteShift) % 256;
			color = palette[paletteIndex];
			setPixel(screen, x, y, RedFromInt(color), GreenFromInt(color), BlueFromInt(color));
		}
	}

	if (SDL_MUSTLOCK(screen))
		SDL_UnlockSurface(screen);

	SDL_Flip(screen);
}

void setPixel(SDL_Surface *screen, Uint32 x, Uint32 y, Uint8 r, Uint8 g, Uint8 b) {
	Uint32 *pixmem32;
	Uint32 color;
	Uint32 yTimesW = y * screen->pitch / (SCREEN_BPP / 8);

	color = SDL_MapRGB(screen->format, r, g, b);

	pixmem32 = (Uint32*)screen->pixels + yTimesW + x;
	*pixmem32 = color;
}