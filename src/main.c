#include "libretro_port.h"
#include "ascii_video.h"
#include "input.h"
#include "tui.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

void debug_run();
bool dummy_environment(unsigned cmd, void *data);
size_t audio_sample_batch_callback(const int16_t *data, size_t frames);

int parse_args(int argc, char *argv[], char* rom_path, char* core_path)
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s <rom_path> <core_path> [.sav path] [FPS]\n", argv[0]);\
        return -1;
    }
    strcpy(rom_path, argv[1]);
    strcpy(core_path, argv[2]);
    return 0;
}


int parse_other_args(int argc, char *argv[], char* game_save_path, int* FPS)
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s <rom_path> <core_path> [.sav path] [FPS]\n", argv[0]);\
        return -1;
    }

    if (argc > 3)
    {
        strcpy(game_save_path, argv[3]);
    }

    if (argc > 4)
    {
        *FPS = atoi(argv[4]);
    }
    return 0;
}

int main(int argc, char *argv[]) 
{
    int FPS = 120;
    char game_save_path[1024] = { 0 };
    char rom_path[1024] = { 0 };
    char core_path[1024] = { 0 };
    if (parse_args(argc, argv, rom_path, core_path) != 0)
        return -1;
    
    if (parse_other_args(argc, argv, game_save_path, &FPS) != 0)
        return -1;

    libretro_callbacks_t callbacks;
    callbacks.retro_audio_sample_batch = audio_sample_batch_callback;
    callbacks.retro_environment = dummy_environment;
    callbacks.retro_input_poll = input_poll_callback;
    callbacks.retro_input_state = input_state_callback;
    callbacks.retro_video_refresh = ascii_video_refresh_ncurses;
    libtretro_port_init(core_path, &callbacks);

    retro_init_t retro_init = NULL;
    retro_run_t retro_run = NULL;
    retro_load_game_t retro_load_game = NULL;
    libtretro_port_get_api(&retro_init, &retro_load_game, &retro_run);

    retro_init();
    retro_game_info game = {
        .path = rom_path,
        .data = NULL,
        .size = 0,
        .meta = NULL
    };

    if (!retro_load_game(&game)) 
    {
        fprintf(stderr, "Failed to load game\n");
        return 1;
    }

    libtretro_port_load_to_sram(game_save_path); // NULL is ok
    tui_init();

    uint64_t save_interval_us = 10 * 1000 * 1000; 
    uint64_t delta_time_us = 1.0 * 1000 * 1000 / FPS;
    uint64_t timer_us = 0;
    while (1) 
    {
    #ifdef _INPUT_DEBUG
        debug_run();
    #else
        retro_run();
        timer_us += delta_time_us;
        if (timer_us >= save_interval_us) 
        {
            // libtretro_port_save_sram(game_save_path);
            timer_us = 0;
        }
    #endif
        usleep(delta_time_us);
    }

    tui_destroy();

    return 0;
}


void debug_run()
{
    input_poll_callback();
}



size_t audio_sample_batch_callback(const int16_t *data, size_t frames)
{
    // no audio output
    return frames;
}

bool dummy_environment(unsigned cmd, void *data)
{
    // dummy 
    return false; 
}