#ifndef __ASCII_VIDEO_H__
#define __ASCII_VIDEO_H__

#include <stddef.h>

void ascii_video_refresh_ncurses(const void *data, unsigned width, unsigned height, size_t pitch);

#endif
