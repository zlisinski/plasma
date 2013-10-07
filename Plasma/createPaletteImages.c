#include <stdio.h>
#include <error.h>
#include <math.h>
#include <gd.h>

// Combine RGB bytes into a RGBA integer
#define RGBToInt(r, g, b) (((r & 0xFF) << 16) + ((g & 0xFF) << 8) + ((b & 0xFF)))

const int IMAGE_WIDTH = 256;
const int IMAGE_HEIGHT = 100;

void populatePalette1(unsigned int palette[]) {
	unsigned int i, r, g, b;
	for (i = 0; i < 256; i++) {
		r = (unsigned int)(127.5 + 128 * sin(3.1415 * i / 128.0));
		g = (unsigned int)(127.5 + 128 * cos(3.1415 * i / 128.0));
		b = (unsigned int)(127.5 + 128 * sin(3.1415 * i / 128.0) * -1);
		palette[i] = RGBToInt(r, g, b);
	}
}

void populatePalette2(unsigned int palette[]) {
	unsigned int i, r, g, b;
	for (i = 0; i < 256; i++) {
		r = 0;
		g = (unsigned int)(127.5 + 128 * cos(3.1415 * i / 128.0));
		b = (unsigned int)(127.5 + 128 * sin(3.1415 * i / 128.0));
		palette[i] = RGBToInt(r, g, b);
	}
}

void createImage(char *filename, unsigned int palette[]) {
	FILE *fp = NULL;
	gdImagePtr img = NULL;
	int color;
	int r, g, b;

	img = gdImageCreateTrueColor(IMAGE_WIDTH, IMAGE_HEIGHT);

	for (int x = 0; x < IMAGE_WIDTH; x++) {
		for (int y = 0; y < IMAGE_HEIGHT; y++) {
			r = gdImageRed(img, palette[x]);
			g = gdImageGreen(img, palette[x]);
			b = gdImageBlue(img, palette[x]);
			color = gdImageColorAllocate(img, r, g, b);
			gdImageSetPixel(img, x, y, color);
		}
	}

	if ((fp = fopen(filename, "w")) == NULL) {
		error(1, 0, "Error - fopen(): %s", filename);
	}
	else {
		gdImagePng(img, fp);
		fclose(fp);
	}

	gdImageDestroy(img);
}

int main(int argc, char *argv[]) {
	unsigned int palette[256] = {0};

	populatePalette1(palette);
	createImage("1.png", palette);

	populatePalette2(palette);
	createImage("2.png", palette);

	return 0;
}
