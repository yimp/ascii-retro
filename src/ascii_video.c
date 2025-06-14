#include "ascii_video.h"
#include <stdio.h>
#include <stdint.h>
#include <ncurses.h>


const char *ascii_palette = " .:-=+*#%@";

char map_luminance_to_ascii(uint8_t lum) 
{
    int palette_size = 10;
    int index = lum * palette_size / 256;
    if (index >= palette_size) index = palette_size - 1;
    return ascii_palette[index];
}

void ascii_video_refresh_ncurses(const void *data, unsigned width, unsigned height, size_t pitch) 
{
    const uint8_t *frame = (const uint8_t *)data;

    int term_rows, term_cols;
    getmaxyx(stdscr, term_rows, term_cols); // get terminal size

    // calculate scale step
    float y_scale = (float)height / term_rows;
    float x_scale = (float)width / term_cols;

    for (int ty = 0; ty < term_rows; ++ty)
    {
        int y = (int)(ty * y_scale);
        if (y >= height) 
            continue;

        for (int tx = 0; tx < term_cols; ++tx) 
        {
            int x = (int)(tx * x_scale);
            if (x >= width) continue;

            size_t idx = y * pitch + x * 2;
            uint16_t pixel = *(uint16_t *)(frame + idx);

            uint8_t r = ((pixel >> 11) & 0x1F) << 3;
            uint8_t g = ((pixel >> 5) & 0x3F) << 2;
            uint8_t b = (pixel & 0x1F) << 3;
            uint8_t lum = (uint8_t)(0.3 * r + 0.59 * g + 0.11 * b);

            char ch = map_luminance_to_ascii(lum);
            mvaddch(ty, tx, ch);
        }
    }

    refresh();
}