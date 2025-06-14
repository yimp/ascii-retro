#include "input.h"
#include <ncurses.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>

/*
* DEFINES below are from libretro.h(https://github.com/libretro/RetroArch/blob/master/libretro-common/include/libretro.h)
*/
/*********************** LIBRETRO PORT BEGIN *********************************/

/* The JOYPAD is called RetroPad. It is essentially a Super Nintendo
 * controller, but with additional L2/R2/L3/R3 buttons, similar to a
 * PS1 DualShock. */
#define RETRO_DEVICE_JOYPAD       1


/* Buttons for the RetroPad (JOYPAD).
 * The placement of these is equivalent to placements on the
 * Super Nintendo controller.
 * L2/R2/L3/R3 buttons correspond to the PS1 DualShock.
 * Also used as id values for RETRO_DEVICE_INDEX_ANALOG_BUTTON */
#define RETRO_DEVICE_ID_JOYPAD_B        0
#define RETRO_DEVICE_ID_JOYPAD_Y        1
#define RETRO_DEVICE_ID_JOYPAD_SELECT   2
#define RETRO_DEVICE_ID_JOYPAD_START    3
#define RETRO_DEVICE_ID_JOYPAD_UP       4
#define RETRO_DEVICE_ID_JOYPAD_DOWN     5
#define RETRO_DEVICE_ID_JOYPAD_LEFT     6
#define RETRO_DEVICE_ID_JOYPAD_RIGHT    7
#define RETRO_DEVICE_ID_JOYPAD_A        8
#define RETRO_DEVICE_ID_JOYPAD_X        9
#define RETRO_DEVICE_ID_JOYPAD_L       10
#define RETRO_DEVICE_ID_JOYPAD_R       11
#define RETRO_DEVICE_ID_JOYPAD_L2      12
#define RETRO_DEVICE_ID_JOYPAD_R2      13
#define RETRO_DEVICE_ID_JOYPAD_L3      14
#define RETRO_DEVICE_ID_JOYPAD_R3      15
/*********************** LIBRETRO PORT END *********************************/

/*
* FIXME:
* These times are not accurate for different terminals.
*/
#define RELEASE_TIMEOUT     150     // ms, timeout for releasing key
#define REPEAT_DELAY        400     // ms, time to wait before repeat
#define REPEAT_INTERVAL     50      // ms, key repeat interval
#define MAX_KEYS (RETRO_DEVICE_ID_JOYPAD_R + 1)
int key_states[MAX_KEYS] = { 0 };
struct timeval key_last_seen[MAX_KEYS];
struct timeval key_down_time[MAX_KEYS];
struct timeval key_last_repeat[MAX_KEYS];
key_event_callback_t _key_press_callback = NULL;
key_event_callback_t _key_release_callback = NULL;
key_event_callback_t _key_repeat_callback = NULL;

void input_set_key_press_callback(key_event_callback_t cb)
{
    assert(cb && !_key_press_callback);
    _key_press_callback = cb;
}

void input_set_key_release_callback(key_event_callback_t cb)
{
    assert(cb && !_key_release_callback);
    _key_release_callback = cb;
}

void input_set_key_repeat_callback(key_event_callback_t cb)
{
    assert(cb && !_key_repeat_callback);
    _key_repeat_callback = cb;
}

int map_key(int key) 
{
    switch (key)
    {
    case 'z': return RETRO_DEVICE_ID_JOYPAD_A;
    case 'x': return RETRO_DEVICE_ID_JOYPAD_B;
    case 's': return RETRO_DEVICE_ID_JOYPAD_SELECT;
    case 'a': return RETRO_DEVICE_ID_JOYPAD_START;
    case 'q': return RETRO_DEVICE_ID_JOYPAD_L;
    case 'w': return RETRO_DEVICE_ID_JOYPAD_R;
    case KEY_UP: return RETRO_DEVICE_ID_JOYPAD_UP;
    case KEY_DOWN: return RETRO_DEVICE_ID_JOYPAD_DOWN;
    case KEY_LEFT: return RETRO_DEVICE_ID_JOYPAD_LEFT;
    case KEY_RIGHT: return RETRO_DEVICE_ID_JOYPAD_RIGHT;
    default: return -1;
    }
    return -1;
}

static long time_diff_ms(struct timeval *a, struct timeval *b) 
{
    return (a->tv_sec - b->tv_sec) * 1000 + (a->tv_usec - b->tv_usec) / 1000;
}

void input_init()
{
    memset(key_states, 0, sizeof(key_states));
    memset(key_last_seen, 0, sizeof(key_last_seen));
    memset(key_down_time, 0, sizeof(key_down_time));
    memset(key_last_repeat, 0, sizeof(key_last_repeat));
}


/*
* FIXME:
* The ncurses 'getch' just reads characters from stdin
* It cannot directly read keyboard events from the operating system event queue, 
* so it cannot perfectly record the status of each frame of keys.
* https://blog.robertelder.org/detect-keyup-event-linux-terminal
*/
void input_poll_callback(void) 
{
    int ch;
    bool currently_pressed[MAX_KEYS] = { false };

    while ((ch = getch()) != ERR) 
    {
        int key = map_key(ch);
        if (key >= 0 && key < MAX_KEYS)
        {
            currently_pressed[key] = true;
        }
    }

    struct timeval now;
    gettimeofday(&now, NULL);

    // update key states
    for (int i = 0; i < MAX_KEYS; ++i)
    {
        if (currently_pressed[i] && key_states[i]) 
        {
            key_states[i] = true;
            key_last_seen[i] = now;
        } 
        else if(currently_pressed[i] && !key_states[i])
        {
            key_states[i] = true;
            key_down_time[i] = now;
            key_last_seen[i] = now;
        }
    }

    // emulate key events
    for (int i = 0; i < MAX_KEYS; ++i)
    {
        if (!key_states[i]) continue;

        long since_last_seen = time_diff_ms(&now, &key_last_seen[i]);
        long held_time       = time_diff_ms(&now, &key_down_time[i]);
        long since_repeat    = time_diff_ms(&now, &key_last_repeat[i]);
        if (held_time == 0)
        {
            if (_key_press_callback) 
                _key_press_callback(i);
            continue;
        }

        if (held_time >= REPEAT_DELAY && since_repeat >= REPEAT_INTERVAL) 
        {
            key_last_repeat[i] = now;
            if (_key_repeat_callback) 
                _key_repeat_callback(i);
            continue;
        }

        if (since_last_seen > RELEASE_TIMEOUT)
        {
            key_states[i] = 0;
            if (_key_release_callback)
                _key_release_callback(i);
            continue;
        }
    }

#ifdef _INPUT_DEBUG
    // output key status
    char buf[MAX_KEYS * 3 + 1] = {0};
    for (int i = 0; i < MAX_KEYS; ++i) {
        strcat(buf, key_states[i] ? "1 " : "0 ");
    }
    // ncurses output
    mvprintw(0, 0, "Input: %s", buf);
#endif
}
int16_t input_state_callback(unsigned port, unsigned device, unsigned index, unsigned id) 
{
    if (port != 0 || device != RETRO_DEVICE_JOYPAD || id >= MAX_KEYS)
        return 0;
    return key_states[id] ? 1 : 0;
}