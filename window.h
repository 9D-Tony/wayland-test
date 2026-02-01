#ifndef WINDOW_H
#define WINDOW_H

uint32_t pixel_alloc;
int32_t allocate_shared_memory(uint64_t sz);

static void resize_window()
{
    int32_t alloc_size = window_width * window_height * 4;
    int32_t fd = allocate_shared_memory(alloc_size );

    pixel = (uint8_t*)mmap(0,alloc_size , PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    //pixel_alloc = (uint32_t*)mmap(0,alloc_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    struct wl_shm_pool *shared_memory_pool = wl_shm_create_pool(shared_memory, fd, alloc_size);
    buffer = wl_shm_pool_create_buffer(shared_memory_pool,0,window_width,window_height,window_width * 4,WL_SHM_FORMAT_ARGB8888);

    wl_shm_pool_destroy(shared_memory_pool);
    close(fd);
}

static void set_clear_color(float red, float green, float blue)
{
    uint8_t alpha = 255;
    uint8_t multiplied_blue = blue * (float)alpha / 256.0f;
    uint8_t multiplied_green = green * (float)alpha / 256.0f;
    uint8_t multiplied_red = red * (float)alpha / 256.0f;

    for (uint32_t i = window_width * window_height * 4; i > 0; i -= 4) {

        //basic alpha blending
        pixel[i] = multiplied_blue;
        pixel[i + 1] = multiplied_green;
        pixel[i + 2] = multiplied_red;
        pixel[i + 3] = alpha; //set alpha
    }
}

void render_box(uint8_t *pixel, uint32_t box_x, uint32_t box_y,
                uint32_t box_width, uint32_t box_height,
                uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) {

    // Premultiply the colors
    uint8_t multiplied_blue = blue * (float)alpha / 256.0f;
    uint8_t multiplied_green = green * (float)alpha / 256.0f;
    uint8_t multiplied_red = red * (float)alpha / 256.0f;

    int32_t start_x = (box_x < window_width) ? box_x : window_width;
    uint32_t start_y = (box_y < window_height) ? box_y : window_height;
    uint32_t end_x = (box_x + box_width < window_width) ? box_x + box_width : window_width;
    uint32_t end_y = (box_y + box_height < window_height) ? box_y + box_height : window_height;

    // Calculate box position (centered)
    // Render the box
    for (uint32_t y = start_y; y < end_y; y++) {
        for (uint32_t x = start_x; x < end_x; x++)
        {
            // Calculate pixel index: (y * width + x) * 4 bytes per pixel
            uint32_t i = (y * window_width + x) * 4;
            pixel[i] = multiplied_blue;
            pixel[i + 1] = multiplied_green;
            pixel[i + 2] = multiplied_red;
            pixel[i + 3] = alpha; // shouldn't overwrite pixels and show past the window.
        }
    }
}

static void draw()
{
    wl_display_flush(display);

    //draw something into the window
    set_clear_color(32,32,32);

    render_box(pixel, window_width / 2, window_height / 2, 50, 50, 64, 64, 64, 255);
    render_box(pixel, mouse.x - 25, mouse.y - 25, 50, 50, 255, 0, 0, 255);
    render_box(pixel, 0, 0, window_width, 32, 64, 64, 64, 255); // draw title bar

    wl_surface_attach(surface,buffer, 0,0);
    wl_surface_damage_buffer(surface, 0,0, window_width, window_height);
    wl_surface_commit(surface);
}

struct wl_callback_listener cb_list;

static void frame_new(void *data, struct wl_callback *cb, uint32_t a)
{
    wl_callback_destroy(cb);
    cb = wl_surface_frame(surface);
    wl_callback_add_listener(cb, &cb_list,0);

    draw();
}

struct wl_callback_listener cb_list = {
    .done = frame_new
};

#endif
