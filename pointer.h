#ifndef POINTER_H
#define POINTER_H

//defined in /usr/include/wayland-client-protocol.h
static void pointer_handle_button(void *data, struct wl_pointer *pointer,
                                  uint32_t serial, uint32_t time, uint32_t button, uint32_t state) {
    struct wl_seat *seat = data;

    // If the user presses the left pointer button, start an interactive move
    // of the toplevel

    if (button == BTN_MIDDLE && state == WL_POINTER_BUTTON_STATE_PRESSED) {
        printf("middle mouse button pressed\n");
    }else if (button == BTN_RIGHT && state == WL_POINTER_BUTTON_STATE_PRESSED) {
        printf("right mouse button pressed\n");
    }else if (button == BTN_LEFT && state == WL_POINTER_BUTTON_STATE_PRESSED) {
        printf("left mouse button pressed\n");
        xdg_toplevel_move(xdg_toplevel, seat, serial);
    }

    if(state == WL_POINTER_BUTTON_STATE_RELEASED)
    {
        printf("button released\n");
    }

    printf("button state: %d \n",state);
}

/*
void *data,
struct wl_pointer *wl_*pointer,
u *int32_t time,
wl_fixed_t surface_x,
wl_fixed_t surface_y);*/

static void pointer_handle_motion(void *data, struct wl_pointer *pointer,
                                uint32_t time,
                                wl_fixed_t surface_x,
                                wl_fixed_t surface_y)
{
    //printf("surface_x: %d, surface_y: %d \n", surface_x,surface_y);
}


/*
void *data,           *
struct wl_pointer *wl_pointer,
uint32_t time,
uint32_t axis,
wl_fixed_t value
*/
static void pointer_handle_scroll(void *data, struct wl_pointer *pointer,
                                  uint32_t time,
                                  uint32_t axis,
                                  wl_fixed_t value)
{
    // wl_fixed_t is just int32_t can convert to double
    // by wl_fixed_to_double function

    printf("axis: %d, value: %lf \n", axis,wl_fixed_to_double(value));
}

#endif
