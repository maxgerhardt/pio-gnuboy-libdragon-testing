/* sys_video_n64.c */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "../gnuboy/fb.h"
#include "../gnuboy/input.h"
#include "../gnuboy/rc.h"
#include <libdragon.h>

#define GBC_LCD_WIDTH 160
#define GBC_LCD_HEIGHT 144

struct fb fb;

int useVideo;

rcvar_t vid_exports[] = {
    RCV_BOOL("usevideo", &useVideo, "Enable video output"),
    RCV_END
};

extern void render();
extern surface_t gb_output_surface;
void convert_rgb565_to_rgba5551(
    const uint16_t *input, uint16_t *output, int width, int height);
uint16_t local_framebuf[GBC_LCD_WIDTH * GBC_LCD_HEIGHT] = {0};

void vid_init()
{
    int w = 0, h = 0, pitch = 0, indexed = 0;
    byte *framebuf = NULL;

    w = GBC_LCD_WIDTH;
    h = GBC_LCD_HEIGHT;
    pitch = GBC_LCD_WIDTH * 2; // length of one row in bytes
    indexed = fb.pelsize == 1;
    framebuf = (byte*) local_framebuf;

    fb.w = w;
    fb.h = h;
    fb.pelsize = 2;   /* bytes per pixel */
    fb.pitch = pitch; /* the length of a row of pixels in bytes. here width * 16 bit*/
    fb.indexed = fb.pelsize == 1;
    fb.ptr = framebuf;
    fb.yuv = 0;
    fb.enabled = 1;

    /* we have a 16-bit R5G6R5 display! */
    fb.cc[0].r = 3;      /* shift amount right for red */
    fb.cc[0].l = 16 - 5; /* shift amount left for red */
    fb.cc[1].r = 2;
    fb.cc[1].l = 16 - 5 - 6;
    fb.cc[2].r = 3;
    fb.cc[2].l = 16 - 5 - 6 - 5;

    return;
}

void vid_setpal(int i, int r, int g, int b)
{
    printf("Setting index %d (%2X,%2X,%2X)\n", i, r, g, b);
}

void vid_preinit()
{
}

void vid_close()
{
}

void vid_settitle(char *title)
{
    printf("vid_settitle: %s.\n", title);
}

void vid_begin()
{
}

void vid_end()
{
    convert_rgb565_to_rgba5551(local_framebuf, (uint16_t*) gb_output_surface.buffer, GBC_LCD_WIDTH, GBC_LCD_HEIGHT);
    render();
}