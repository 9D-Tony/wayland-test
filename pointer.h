#ifndef POINTER_H
#define POINTER_H

typedef struct {
    float x;
    float y;
    uint32_t grabbing;
    uint32_t is_entered;
    uint32_t button_pressed;
}Mouse;

Mouse mouse;

//defined in /usr/include/wayland-client-protocol.h
static void pointer_handle_button(void *data, struct wl_pointer *pointer,
                                  uint32_t serial, uint32_t time, uint32_t button, uint32_t state) {
    struct wl_seat *seat = data;

    // If the user presses the left pointer button, start an interactive move
    // of the toplevel

    if (button == BTN_MIDDLE && state == WL_POINTER_BUTTON_STATE_PRESSED) {
        mouse.button_pressed = BTN_MIDDLE;
    }else if (button == BTN_RIGHT && state == WL_POINTER_BUTTON_STATE_PRESSED) {
        mouse.button_pressed = BTN_RIGHT;
    }else if (button == BTN_LEFT && state == WL_POINTER_BUTTON_STATE_PRESSED) {

        if(mouse.y < 32.0f) // only draw
        {
            xdg_toplevel_move(xdg_toplevel, seat, serial); // this make the mouse leave the window so release is never called.
        }

        mouse.grabbing = true;
        mouse.button_pressed = BTN_LEFT;
    }

}

static void pointer_handle_motion(void *data, struct wl_pointer *pointer,
                                uint32_t time,
                                wl_fixed_t surface_x,
                                wl_fixed_t surface_y)
{
    //printf("surface_x: %lf, surface_y: %lf \n", wl_fixed_to_double(surface_x),wl_fixed_to_double(surface_y));
    mouse.x = (float)wl_fixed_to_double(surface_x);
    mouse.y = (float)wl_fixed_to_double(surface_y);
}

static void pointer_handle_enter(void *data, struct wl_pointer *pointer,
                                 uint32_t serial,
                                 struct wl_surface *surface,
                                 wl_fixed_t surface_x,
                                 wl_fixed_t surface_y)
{
    mouse.is_entered = true;
    if(mouse.grabbing)
    {
        mouse.grabbing = false;
        mouse.button_pressed = BTN_DEAD;
    }
}

static void pointer_handle_leave(void *data, struct wl_pointer *pointer,
                                  uint32_t serial,
                                  struct wl_surface *surface)
{
    mouse.is_entered = false;
}

static void pointer_handle_scroll(void *data, struct wl_pointer *pointer,
                                  uint32_t time,
                                  uint32_t axis,
                                  wl_fixed_t value)
{

    printf("axis: %d, value: %lf \n", axis,wl_fixed_to_double(value));
}

#endif
