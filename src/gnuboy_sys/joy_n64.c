#include "../gnuboy/input.h"
#include "../gnuboy/rc.h"
#include <libdragon.h>

rcvar_t joy_exports[] =
    {
        RCV_END};

void joy_init() { }

void joy_close() { }

void joy_poll() {}

#define POST_EVENT_IF(cond, keycode) \
  if(cond) { \
    ev.code = (keycode); \
    ev_postevent(&ev); \
  }

void ev_poll(int wait)
{
    event_t ev = {};
    joypad_poll();
    joypad_buttons_t pressed_btns = joypad_get_buttons_pressed(JOYPAD_PORT_1);

    ev.type = EV_PRESS;
    POST_EVENT_IF(pressed_btns.start, K_ENTER); // START
    POST_EVENT_IF(pressed_btns.c_up, K_SPACE); // SELECT
    POST_EVENT_IF(pressed_btns.a, K_JOY1); // A
    POST_EVENT_IF(pressed_btns.b, K_JOY0); // B
    POST_EVENT_IF(pressed_btns.d_left, K_LEFT); // LEFT
    POST_EVENT_IF(pressed_btns.d_right, K_RIGHT); // RIGHT
    POST_EVENT_IF(pressed_btns.d_up, K_UP); // UP
    POST_EVENT_IF(pressed_btns.d_down, K_DOWN); // DOWN

    joypad_buttons_t released_btns = joypad_get_buttons_released(JOYPAD_PORT_1);
    ev.type = EV_RELEASE;
    POST_EVENT_IF(released_btns.start, K_ENTER);
    POST_EVENT_IF(released_btns.c_up, K_SPACE);
    POST_EVENT_IF(released_btns.a, K_JOY1);
    POST_EVENT_IF(released_btns.b, K_JOY0);
    POST_EVENT_IF(released_btns.d_left, K_LEFT);
    POST_EVENT_IF(released_btns.d_right, K_RIGHT);
    POST_EVENT_IF(released_btns.d_up, K_UP);
    POST_EVENT_IF(released_btns.d_down, K_DOWN);
}
