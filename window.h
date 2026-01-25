#ifndef WINDOW_H
#define WINDOW_H

uint32_t pixel_alloc;

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

static void draw_square(uint32_t x,uint32_t y, uint32_t size_x,uint32_t size_y)
{

    uint8_t alpha = 255;
    uint8_t multiplied_blue = 0 * (float)alpha / 256.0f;
    uint8_t multiplied_green = 0 * (float)alpha / 256.0f;
    uint8_t multiplied_red = 255 * (float)alpha / 256.0f;


    assert(size_x < window_width + 1);
    assert(size_y < window_height + 1);

    for (uint32_t i = size_y * size_x * 4; i < window_width * window_height * 4; i += 4) {

        if(i < size_y * size_x + x * 4)
        {
            //basic alpha blending
            pixel[i] = multiplied_blue;
            pixel[i + 1] = multiplied_green;
            pixel[i + 2] = multiplied_red;
            pixel[i + 3] = alpha; //set alpha
        }
    }
}

static void draw()
{
    //draw something into the window
    //in format BGRA for some reason

    set_clear_color(32,32,32);
    draw_square(10,10,300,300);

    //memset(pixel, 0,window_width*window_height*4);

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
