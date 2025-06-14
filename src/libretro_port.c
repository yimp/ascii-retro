
#include "libretro_port.h"
#include <stddef.h>
#include <assert.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

static int libtretro_port_initialize = 0;
static void* libtretro_port_core_handle = NULL;

void libtretro_port_init(const char* core_path, const libretro_callbacks_t* callbacks)
{
    assert(libtretro_port_initialize == 0);
    void* handle = dlopen(core_path, RTLD_LAZY);
    assert(handle);

    retro_set_video_refresh_t set_video_refresh = dlsym(handle, RETRO_API_NAME_SET_VIDEO_REFRESH);
    retro_set_environment_t set_environment = dlsym(handle, RETRO_API_NAME_SET_ENVIRONMENT);
    retro_set_input_poll_t set_input_poll = dlsym(handle, RETRO_API_NAME_SET_INPUT_POLL);
    retro_set_input_state_t set_input_state = dlsym(handle, RETRO_API_NAME_SET_INPUT_STATE);
    retro_set_audio_sample_batch_t set_audio_sample_batch = dlsym(handle, RETRO_API_NAME_SET_AUDIO_SAMPLE_BATCH);
    assert(!(!set_video_refresh || !set_environment || !set_input_poll 
        || !set_input_state || !set_audio_sample_batch));

    set_environment(callbacks->retro_environment);
    set_video_refresh(callbacks->retro_video_refresh);
    set_input_poll(callbacks->retro_input_poll);
    set_input_state(callbacks->retro_input_state);
    set_audio_sample_batch(callbacks->retro_audio_sample_batch);

    libtretro_port_core_handle = handle;
    libtretro_port_initialize = 1;
}

void libtretro_port_get_api(retro_init_t*  retro_init, retro_load_game_t* retro_load_game, retro_run_t* retro_run)
{
    assert(libtretro_port_initialize == 1);
    assert(retro_init && retro_run && retro_load_game);
    *retro_init = dlsym(libtretro_port_core_handle, RETRO_API_NAME_INIT);
    *retro_run = dlsym(libtretro_port_core_handle, RETRO_API_NAME_RUN);
    *retro_load_game = dlsym(libtretro_port_core_handle, RETRO_API_NAME_LOAD_GAME);
    assert(*retro_init && *retro_run && *retro_load_game);
}


#define RETRO_MEMORY_SAVE_RAM 0
#define RETRO_API_NAME_GET_MEMORY_DATA "retro_get_memory_data"
#define RETRO_API_NAME_GET_MEMORY_SIZE "retro_get_memory_size"
#define RETRO_API_NAME_SET_AUDIO_SAMPLE_BATCH "retro_set_audio_sample_batch"
typedef void*   (*retro_get_memory_data_t)(unsigned);
typedef size_t  (*retro_get_memory_size_t)(unsigned);
void libtretro_port_load_to_sram(const char* game_sav_path)
{
    if (game_sav_path == NULL) return;
    if (strlen(game_sav_path) == 0) return;
    assert(libtretro_port_initialize == 1);
    retro_get_memory_data_t retro_get_memory_data = NULL;
    retro_get_memory_size_t retro_get_memory_size = NULL;
    retro_get_memory_data = dlsym(libtretro_port_core_handle, RETRO_API_NAME_GET_MEMORY_DATA);
    retro_get_memory_size = dlsym(libtretro_port_core_handle, RETRO_API_NAME_GET_MEMORY_SIZE);
    assert(retro_get_memory_data && retro_get_memory_size);

    void *sram = retro_get_memory_data(RETRO_MEMORY_SAVE_RAM);
    size_t size = retro_get_memory_size(RETRO_MEMORY_SAVE_RAM);
    assert(sram);
    assert(size);

    // open file
    int fd = open(game_sav_path, O_RDONLY);
    assert(fd > 0);

    size_t bytes_read = read(fd, sram, size);
    assert(bytes_read > 0);
    close(fd);
}


void libtretro_port_save_sram(const char* game_sav_path)
{
    if (game_sav_path == NULL) return;
    if (strlen(game_sav_path) == 0) return;
    assert(libtretro_port_initialize == 1);
    retro_get_memory_data_t retro_get_memory_data = NULL;
    retro_get_memory_size_t retro_get_memory_size = NULL;
    retro_get_memory_data = dlsym(libtretro_port_core_handle, RETRO_API_NAME_GET_MEMORY_DATA);
    retro_get_memory_size = dlsym(libtretro_port_core_handle, RETRO_API_NAME_GET_MEMORY_SIZE);
    assert(retro_get_memory_data && retro_get_memory_size);

    void *sram = retro_get_memory_data(RETRO_MEMORY_SAVE_RAM);
    size_t size = retro_get_memory_size(RETRO_MEMORY_SAVE_RAM);
    assert(sram);
    assert(size);

    // open file
    int fd = open(game_sav_path, O_WRONLY);
    assert(fd > 0);

    size_t bytes_write = write(fd, sram, size);
    assert(bytes_write > 0);
    close(fd);
}
