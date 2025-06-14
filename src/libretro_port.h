#ifndef __LIBRETRO_PORT_H__
#define __LIBRETRO_PORT_H__
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// Libretro API function pointers
typedef struct {
    const char *path;
    const void *data;
    size_t size;
    const char *meta;
} retro_game_info;

typedef void    (*retro_video_refresh_t)(const void *, unsigned, unsigned, size_t);
typedef bool    (*retro_environment_t)(unsigned, void *);
typedef void    (*retro_input_poll_t)(void);
typedef int16_t (*retro_input_state_t)(unsigned, unsigned, unsigned, unsigned);
typedef size_t  (*retro_audio_sample_batch_t)(const int16_t *, size_t );

typedef void (*retro_init_t)(void);
typedef void (*retro_run_t)(void);
typedef bool (*retro_load_game_t)(const retro_game_info *);
typedef void (*retro_set_video_refresh_t)(retro_video_refresh_t);
typedef void (*retro_set_environment_t)(retro_environment_t);
typedef void (*retro_set_input_poll_t)(retro_input_poll_t);
typedef void (*retro_set_input_state_t)(retro_input_state_t);
typedef void (*retro_set_audio_sample_batch_t)(retro_audio_sample_batch_t);


#define RETRO_API_NAME_INIT "retro_init"
#define RETRO_API_NAME_RUN "retro_run"
#define RETRO_API_NAME_LOAD_GAME "retro_load_game"
#define RETRO_API_NAME_SET_VIDEO_REFRESH "retro_set_video_refresh"
#define RETRO_API_NAME_SET_ENVIRONMENT "retro_set_environment"
#define RETRO_API_NAME_SET_INPUT_POLL "retro_set_input_poll"
#define RETRO_API_NAME_SET_INPUT_STATE "retro_set_input_state"
#define RETRO_API_NAME_SET_AUDIO_SAMPLE_BATCH "retro_set_audio_sample_batch"


typedef struct
{
    retro_video_refresh_t       retro_video_refresh;
    retro_environment_t         retro_environment;
    retro_input_poll_t          retro_input_poll;
    retro_input_state_t         retro_input_state;
    retro_audio_sample_batch_t  retro_audio_sample_batch;
} libretro_callbacks_t;

void libtretro_port_init(const char* core_path, const libretro_callbacks_t* callbacks);
void libtretro_port_get_api(retro_init_t*  retro_init, retro_load_game_t* retro_load_game, retro_run_t* retro_run);
void libtretro_port_load_to_sram(const char* game_sav_path);
void libtretro_port_save_sram(const char* game_sav_path);
#endif
