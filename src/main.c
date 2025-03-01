#include "libdragon.h"
#include <malloc.h>
#include <math.h>
#include <../star_image.h>

extern int gnuboy_main(int argc, char *argv[]);

#define GB_RESOLUTION_WIDTH 160
#define GB_RESOLUTION_HEIGHT 144 

static int32_t cur_tick = 0;
static uint32_t display_width;
static uint32_t display_height;
static uint8_t* gb_rom_data = NULL;
static int gb_rom_data_len = 0;

enum {
    STYLE_ID_WHITE = 0
};
enum {
    FONT_ID_DEBUG_MONO = 1
};


void convert_rgb565_to_rgba5551(
    const uint16_t *input, uint16_t *output, int width, int height) {
    for (int i = 0; i < width * height; i++) {
        uint16_t pixel = input[i];
        
        // Extract RGB565 components
        uint8_t r = (pixel >> 11) & 0x1F;
        uint8_t g = (pixel >> 5) & 0x3F;
        uint8_t b = pixel & 0x1F;
        
        // Convert to RGBA5551 using the macro
        color_t rgba = RGBA16(r, g >> 1, b, 1);
        
        // Pack back into 16-bit format (5R, 5G, 5B, 1A)
        output[i] = color_to_packed16(rgba);
    }
}

void update(int ovfl)
{
    cur_tick++;
}

surface_t gb_output_surface;

static rspq_block_t *gb_framebuf_block;


void render()
{
    // manipulate surface / buffer using graphics API, just as a test
    //graphics_fill_screen(&gb_output_surface, graphics_make_color (255, 255, 255, 255));
    //uint32_t gr_green_col = graphics_make_color (0, 255, 0, 255);
    //graphics_draw_line(&gb_output_surface, 0, 0, GB_RESOLUTION_WIDTH, GB_RESOLUTION_HEIGHT, gr_green_col);
    
    // Attach and clear the screen
    surface_t *disp = display_get();
    rdpq_attach_clear(disp, NULL);
    // start with a red screen
    //color_t clear_color = {.r = 255, .b = 0, .g = 0, .a = 0};
    //rdpq_clear(clear_color);

    rspq_block_run(gb_framebuf_block);

    // render texture to screen (fast but unscaled)
    //rdpq_set_mode_copy(false);
    //rdpq_tex_blit(&gb_output_surface, (display_width - gb_output_surface.width) / 2, (display_height - gb_output_surface.height) / 2, NULL);
    // render texture to screen (slow but scaled)
    /*rdpq_set_mode_standard();
    rdpq_tex_blit(&gb_output_surface, (display_width - gb_output_surface.width) / 4, (display_height - gb_output_surface.height) / 4, &(rdpq_blitparms_t){
        .scale_x = 2,
        .scale_y = 2
    });
    */
    //static const rdpq_textparms_t draw_center = { .align = ALIGN_CENTER };
    // example text
    //rdpq_text_printf(NULL, FONT_ID_DEBUG_MONO, display_width / 2 - 90, (int)(0.95f*display_height), "FPS %.2f Refresh %.2f Hz %d", display_get_fps(), display_get_refresh_rate(), (int) get_tv_type());

    rdpq_detach_show();
    //rdpq_detach_wait();
    //display_show(disp);
}


void record_rendering_block() {
    rspq_block_begin();

    rdpq_set_mode_copy(false);
    rdpq_tex_blit(&gb_output_surface, (display_width - gb_output_surface.width) / 2, (display_height - gb_output_surface.height) / 2, NULL);

    gb_framebuf_block = rspq_block_end();
}

int main()
{
    debug_init_isviewer();
    debug_init_usblog();

    //display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE);
    const resolution_t RESOLUTION_640x480p = {.width = 640, .height = 480, .interlaced = INTERLACE_OFF};
    const resolution_t RESOLUTION_512x384i = {.width = 512, .height = 384, .interlaced = INTERLACE_HALF};
    const resolution_t RESOLUTION_512x384p = {.width = 512, .height = 384, .interlaced = INTERLACE_FULL};

    //if (get_tv_type() == TV_NTSC)
    //    display_init(RESOLUTION_512x384i, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_DISABLED);
    //else
    //    display_init(RESOLUTION_512x384p, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_DISABLED);
    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE);

    joypad_init();
    timer_init();

    display_width = display_get_width();
    display_height = display_get_height();
    display_set_fps_limit(60.0);
    
    dfs_init(DFS_DEFAULT_LOCATION);

    rdpq_init();
    // rdpq_debug_start();
    //gb_output_surface = surface_make_linear(gb_framebuf, FMT_RGBA16, GB_RESOLUTION_WIDTH, GB_RESOLUTION_HEIGHT);
    // allocate a new surface for it
    gb_output_surface = surface_alloc(FMT_RGBA16, GB_RESOLUTION_WIDTH, GB_RESOLUTION_HEIGHT);
    convert_rgb565_to_rgba5551(star_image, (uint16_t*) gb_output_surface.buffer, IMAGE_WIDTH, IMAGE_HEIGHT);

    rdpq_font_t* font = rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_MONO);
    rdpq_font_style(font, STYLE_ID_WHITE, &(rdpq_fontstyle_t){ 
        .color = RGBA32(255, 255, 255, 255),
    });
    rdpq_text_register_font(FONT_ID_DEBUG_MONO, font);

    record_rendering_block();

    //update(0);
    //new_timer(TIMER_TICKS(1000000 / 60), TF_CONTINUOUS, update);

    int cur_frame = 0;

    // gb_rom_data = (uint8_t*) asset_load("rom://tetris.gb", &gb_rom_data_len);
    // printf("ROM content (first 256 B):\n");
    // debug_hexdump(gb_rom_data, 256);
    // free(gb_rom_data);
    // gb_rom_data = NULL;
    // gb_rom_data_len = 0;

    // FILE* fp = fopen("rom://tetris.gb", "rb");
    // char readBuf[256] = {0};
    // if(!fp) {
    //     printf("File not found\n");
    // } else {
    //     ssize_t num_read = fread(readBuf, 1, sizeof(readBuf), fp);
    //     debug_hexdump(readBuf, (int) num_read);
    //     fclose(fp);
    // }
    
    char* gnuboy_args[]= {
        "gnuboy", /* path to "program" */
        //"rom://tetris.gb" /* rom path */
        //"rom://pkred.gb" /* rom path */
        "rom://pkyellow.gbc" /* rom path */
    };
    int num_gnuboy_args = sizeof(gnuboy_args) / sizeof(gnuboy_args[0]);
    gnuboy_main(num_gnuboy_args, gnuboy_args);
    // not reached anymore
    while (1)
    {
        render();

        joypad_poll();
        joypad_buttons_t ckeys = joypad_get_buttons_pressed(JOYPAD_PORT_1);

        cur_frame++;
    }
}
