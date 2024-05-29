#include </usr/include/cairo/cairo.h>

cairo_surface_t* init_image(int width, int height);
void draw_title(const char *filename, const char *text, cairo_surface_t *surface);
void draw_body(const char *filename, int seeds, int peers, int completed, int file_count, const char *file_size, int tracker_count, int time, cairo_surface_t *surface);
