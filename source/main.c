#include <stdlib.h>
#include <3ds.h>
#include <sf2d.h>
#include <sftd.h>
#include "FreeSerif_ttf.h"

float f_max(float a, float b) {
	return a>b ? a:b;
}

int main()
{
	sf2d_init();
	sf2d_set_clear_color(RGBA8(0,0,0, 255));
	sf2d_set_vblank_wait(0);

	sftd_init();
	sftd_font* font = sftd_load_font_mem(FreeSerif_ttf, FreeSerif_ttf_size);

	// set font height to something then deduce max width and array dimensions
	uint8_t fheight = 10;
	uint8_t fwidth = 0;
	for (uint16_t i=0; i<256; i++) {
		char s[] = "a";
		s[0] = i;
		uint8_t w = sftd_get_text_width(font, fheight, s);
		if (w>fwidth) fwidth = w;
	}
	uint8_t height = 240/fheight;
	uint8_t width = 400/fwidth;
	uint16_t nb = width*height;

	// character to display at the x,y coordinates
	char chars[nb];
	for (uint16_t i=0; i<nb; i++) chars[i] = rand();

	// opacity of the first character in the x column
	char opacity[width];
	for (uint16_t i=0; i<width; i++) opacity[i] = rand();

	u64 oldtime = 0;

	while (aptMainLoop()) {
		hidScanInput();
		if (hidKeysDown() & KEY_START) break;

		u64 newtime = osGetTime();
		if (newtime-oldtime < 50) continue;
		oldtime = newtime;

		for (int i=0; i<20; i++) chars[((u64)(rand()))%nb] = rand(); // spicier if some chars randomly change
		for (uint16_t i=0; i<width; i++) opacity[i]--;
		sf2d_start_frame(GFX_TOP, GFX_LEFT);
		{
			for (uint8_t x=0; x<width; x++) for (uint8_t y=0; y<height; y++) {
				uint8_t op = opacity[x]+y;
				uint32_t color = RGBA8(255,255,255,255);
				if (op != 255) {
					float x1=255, y1=255, x2=200, y2=0;
					op = f_max(0, (y2-y1)/(x2-x1) * (op-x1) + y1);
					color = RGBA8(0,255,0,op);
				}
				sftd_draw_textf(font, x*fwidth, y*fheight, color, fheight, "%c", chars[y*width+x]);
			}
		}
		sf2d_end_frame();

		sf2d_swapbuffers();
	}

	sftd_free_font(font);
	sftd_fini();

	sf2d_fini();

	return 0;
}
