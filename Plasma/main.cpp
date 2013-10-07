#include <cmath>
#include "SDL.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

void populatePalette1(Uint32 palette[]);
void populatePalette2(Uint32 palette[]);

void DrawScreen(SDL_Surface *screen, int frame, Uint32 palette[], Uint32 **plasma);
void setPixel(SDL_Surface *screen, int x, int y, Uint8 r, Uint8 g, Uint8 b);

// Combine RGB bytes into a RGBA integer
#define RGBToInt(r, g, b) ((((r & 0xFF) << 24) + ((g & 0xFF) << 16) + ((b & 0xFF) << 8)) & 0xFFFFFF00)
// Extract color bytes from a RGBA integer
#define RedFromInt(color) (color >> 24)
#define GreenFromInt(color) ((color & 0x00FF0000) >> 16)
#define BlueFromInt(color) ((color & 0x0000FF00) >> 8)

int main(int argc, char* args[]) {
	Uint32 **plasma;
	Uint32 palette[256];

	SDL_Surface *screen;
	SDL_Event event;

	int keypress = 0;
	int frame = 0; 

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		return 1;

	//Set the window caption
	SDL_WM_SetCaption("Plasma Test", NULL);

	screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE);
	if (screen == NULL) {
		SDL_Quit();
		return 1;
	}

	plasma = new Uint32 *[SCREEN_WIDTH];
	for (int i = 0; i < SCREEN_WIDTH; i++) {
		plasma[i] = new Uint32[SCREEN_HEIGHT];
	}

	//populatePalette1(palette);
	populatePalette2(palette);

	for (int x = 0; x < SCREEN_WIDTH; x++) {
		for (int y = 0; y < SCREEN_HEIGHT; y++)
		{
			int color = (int)(
				128.0 + (128.0 * sin(x / 16.0)) +
				128.0 + (128.0 * sin(y / 8.0)) +
				128.0 + (128.0 * sin((x + y) / 16.0)) +
				128.0 + (128.0 * sin(sqrt(double(x * x + y * y)) / 8.0))
			) / 4;
			plasma[x][y] = color;       
		}
	}

	while (!keypress) {
		DrawScreen(screen, frame++, palette, plasma);
		while (SDL_PollEvent(&event)) {
			switch (event.type)	{
				case SDL_QUIT:
					keypress = 1;
					break;
				case SDL_KEYDOWN:
					keypress = 1;
					break;
			}
		}
	}

	for (int i = 0; i < SCREEN_WIDTH; i++) {
		delete [] plasma[i];
	}
	delete [] plasma;

	SDL_Quit();
  
	return 0;
}

void populatePalette1(unsigned int palette[]) {
	int r, g, b;

	for (int i = 0; i < 256; i++) {
		r = int(127.5 + 128 * sin(3.1415 * i / 128.0));
		g = int(127.5 + 128 * cos(3.1415 * i / 128.0));
		b = int(127.5 + 128 * sin(3.1415 * i / 128.0) * -1);
		palette[i] = RGBToInt(r, g, b);
	}
}

void populatePalette2(unsigned int palette[]) {
	int r, g, b;

	for (int i = 0; i < 256; i++) {
		r = 0;
		g = int(127.5 + 128 * cos(3.1415 * i / 128.0));
		b = int(127.5 + 128 * sin(3.1415 * i / 128.0));
		palette[i] = RGBToInt(r, g, b);
	}
}

int getPlasmaPixel(int x, int y, int frame) {
	int color = (int)(
		128.0 + (128.0 * sin(x / 16.0)) +
		128.0 + (128.0 * sin(y / 8.0)) +
		128.0 + (128.0 * sin((x + y) / 16.0)) +
		128.0 + (128.0 * sin(sqrt(double(x * x + y * y)) / 8.0))
	) / 4;

	return color;
}

void DrawScreen(SDL_Surface *screen, int frame, Uint32 palette[], Uint32 *plasma[]) {
	int x, y, ytimesw;

	if (SDL_MUSTLOCK(screen)) {
		if (SDL_LockSurface(screen) < 0)
			return;
	}

	int paletteShift = frame / 2;

	for (y = 0; y < SCREEN_HEIGHT; y++) {
		ytimesw = y * screen->pitch / (SCREEN_BPP / 8);
		for (x = 0; x < SCREEN_WIDTH; x++) {
			int color = palette[(plasma[x][y] + paletteShift) % 256];
			//int color = palette[(getPlasmaPixel(x, y, frame) + paletteShift) % 256];
			setPixel(screen, x, ytimesw, RedFromInt(color), GreenFromInt(color), BlueFromInt(color));
		}
	}

	if (SDL_MUSTLOCK(screen))
		SDL_UnlockSurface(screen);

	SDL_Flip(screen);
}

void setPixel(SDL_Surface *screen, int x, int y, Uint8 r, Uint8 g, Uint8 b)
{
	Uint32 *pixmem32;
	Uint32 color;  

	color = SDL_MapRGB(screen->format, r, g, b);

	pixmem32 = (Uint32*)screen->pixels + y + x;
	*pixmem32 = color;
}