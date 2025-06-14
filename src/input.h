#ifndef __INPUT_H__
#define __INPUT_H__ 
#include <stdint.h>

void input_init(void);
void input_poll_callback(void);
int16_t input_state_callback(unsigned port, unsigned device, unsigned index, unsigned id);


// virtual keyboard event
typedef void (*key_event_callback_t)(int);
void input_set_key_press_callback(key_event_callback_t cb);
void input_set_key_release_callback(key_event_callback_t cb);
void input_set_key_repeat_callback(key_event_callback_t cb);

#endif