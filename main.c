#include <wayland-client.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <linux/input-event-codes.h> //for handling mouse button codes
#include "xdg-shell-client-protocol.h"

struct wl_compositor *comp;
struct wl_surface *surface;
struct wl_buffer *buffer;
struct wl_shm *shared_memory;
struct xdg_wm_base *xdg_wm_base = NULL;
struct xdg_toplevel *xdg_toplevel = NULL; // acts like a window
struct wl_seat *seat;
struct wl_keyboard *keyboard;
struct wl_pointer *pointer;
struct wl_display *display;

uint8_t pixel_colour = 255;
uint8_t window_close;
uint8_t *pixel;
uint16_t window_width = 800;
uint16_t window_height = 600;

#include "pointer.h"
#include "window.h"
#include "keyboard.h"


int32_t allocate_shared_memory(uint64_t sz)
{
    int8_t name[8];
    name[0] = '/';
    name[7] = 0;
    for (uint8_t i = 1; i < 6; i++)
    {
        name[i] = (rand() & 23) + 97;
    }

    int32_t fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, 0600);
    shm_unlink(name);
    ftruncate(fd,sz);

    return fd;

}

static void noop() {
    // This space intentionally left blank
}

void xrfc_config(void* data, struct xdg_surface* xrfc, uint32_t ser)
{
    xdg_surface_ack_configure(xrfc, ser);

    if(!pixel)
    {
        resize_window();
    }

    draw();
}

struct xdg_surface_listener xrfc_list = {
    .configure = xrfc_config
};

void top_conf(void *data, struct xdg_toplevel *top, int32_t nw ,int32_t nh, struct wl_array *state)
{
    if(!nw && !nh)
    {
        return;
    }

    if(window_width != nw || window_height != nh)
    {
        munmap(pixel,window_width * window_height * 4);
        window_width = nw;
        window_height = nh;
        resize_window();
    }
}

void top_close(void *data, struct xdg_toplevel *top)
{
    window_close = 1;

}

struct xdg_toplevel_listener top_list = {
    .configure = top_conf,
    .close = top_close
};


void sh_ping(void *data,struct xdg_wm_base *sh,uint32_t ser)
{
    xdg_wm_base_pong(sh,ser);
}

struct xdg_wm_base_listener sh_list = {
    .ping = sh_ping
};

static const struct wl_pointer_listener pointer_listener = {
    .enter = pointer_handle_enter,
    .leave = pointer_handle_leave,
    .motion = pointer_handle_motion,
    .button = pointer_handle_button,
    .axis = pointer_handle_scroll,
};


void seat_cap(void *data, struct wl_seat *seat, uint32_t cap)
{
    if (cap & WL_SEAT_CAPABILITY_KEYBOARD && !keyboard) {
        keyboard = wl_seat_get_keyboard(seat);
        wl_keyboard_add_listener(keyboard, &keyboard_list, 0);
    }

    if (cap & WL_SEAT_CAPABILITY_POINTER && !pointer) {
        pointer = wl_seat_get_pointer(seat);
        wl_pointer_add_listener(pointer, &pointer_listener, seat);
    }
}

void seat_name(void* data, struct wl_seat *seat, const char *name)
{
    //ignore
}

struct wl_seat_listener seat_list = {
    .capabilities = seat_cap,
    .name = seat_name
};

static void
registry_handle_global(void *data, struct wl_registry *registry,
                       uint32_t name, const char *interface, uint32_t version)
{
    printf("interface: '%s', version: %d, name: %d\n",
           interface, version, name);

    if(!strcmp(interface, wl_compositor_interface.name))
    {
        comp = wl_registry_bind(registry,name,&wl_compositor_interface,4);
    }else if(!strcmp(interface, wl_shm_interface.name))
    {
         shared_memory = wl_registry_bind(registry,name,&wl_shm_interface,1);

    }else if(!strcmp(interface, xdg_wm_base_interface.name))
    {
        xdg_wm_base = wl_registry_bind(registry,name,&xdg_wm_base_interface,1);
        xdg_wm_base_add_listener(xdg_wm_base, &sh_list,0);

    }else if(!strcmp(interface, wl_seat_interface.name))
    {
        seat = wl_registry_bind(registry,name,&wl_seat_interface,1);
        wl_seat_add_listener(seat, &seat_list,0);
    }
}

static void
registry_handle_global_remove(void *data, struct wl_registry *registry,
                              uint32_t name)
{
    // left blank
}

static const struct wl_registry_listener
registry_listener = {
    .global = registry_handle_global,
    .global_remove = registry_handle_global_remove
};

int
main(int argc, char *argv[])
{

    display = wl_display_connect(0);
    struct wl_registry *registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &registry_listener, 0);
    wl_display_roundtrip(display);

    if (!display)
    {
        fprintf(stderr, "Failed to connect to Wayland display.\n");
        return 1;
    }

    fprintf(stderr, "Display connection successful!\n");

    surface = wl_compositor_create_surface(comp);
    struct wl_callback *cb = wl_surface_frame(surface);
    wl_callback_add_listener(cb,&cb_list,0);


    if(surface == NULL)
    {
        printf("falied ot create surface");
    }

    struct xdg_surface *xrfc = xdg_wm_base_get_xdg_surface(xdg_wm_base,surface);

    xdg_surface_add_listener(xrfc, &xrfc_list, 0);
    xdg_toplevel = xdg_surface_get_toplevel(xrfc);

    xdg_toplevel_add_listener(xdg_toplevel, &top_list, 0);
    xdg_toplevel_set_title(xdg_toplevel,"wayland client");
    wl_surface_attach(surface, buffer, 0, 0);
    wl_surface_commit(surface);

    while(wl_display_dispatch(display))
    {
        if(window_close) break;
    }

    if(keyboard) {
        wl_keyboard_destroy(keyboard);
    }

    if(pointer)
    {
        wl_pointer_destroy(pointer);
    }

    wl_seat_release(seat);
    if(buffer)
    {
        wl_buffer_destroy(buffer);
    }

    xdg_toplevel_destroy(xdg_toplevel);
    xdg_surface_destroy(xrfc);
    wl_surface_destroy(surface);
    wl_display_disconnect(display);
    return 0;
}

