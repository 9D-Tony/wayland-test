#ifndef KEYBOARD_H
#define KEYBOARD_H



static void keyboard_map(void* data, struct wl_keyboard* keyboard, uint32_t frmt, int32_t fd, uint32_t sz) {

}

static void keyboard_enter(void* data, struct wl_keyboard* keyboard, uint32_t ser, struct wl_surface* srfc, struct wl_array* keys) {

}

static void keyboard_leave(void* data, struct wl_keyboard* keyboard, uint32_t ser, struct wl_surface* srfc) {

}

static void keyboard_key(void* data, struct wl_keyboard* keyboard, uint32_t ser, uint32_t t, uint32_t key, uint32_t stat) {

    printf("key pressed: %d\n",key);

    if (key == 1) { //If escape pressed
        window_close = 1;
    }
    else if (key == 30) {
        printf("a\n");
    }
    else if (key == 32) {
        printf("d\n");
    }
}

static void keyboard_mod(void* data, struct wl_keyboard* keyboard, uint32_t ser, uint32_t dep, uint32_t lat, uint32_t lock, uint32_t grp) {

}

static void keyboard_rep(void* data, struct wl_keyboard* keyboard, int32_t rate, int32_t del) {

}

struct wl_keyboard_listener keyboard_list = {
    .keymap = keyboard_map,
    .enter = keyboard_enter,
    .leave = keyboard_leave,
    .key = keyboard_key,
    .modifiers = keyboard_mod,
    .repeat_info = keyboard_rep
};

#endif
