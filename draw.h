#include <cairo/cairo.h>

typedef struct {
    const char *option;
    const char *filename;
    const double scale_factor;
} AssetMapping;

extern const AssetMapping asset_mappings[];
extern const int num_assets;

const char* get_asset_filename(const char *option);
cairo_surface_t* init_image(int width, int height, const char *asset);
void draw_title(const char *filename, const char *text, cairo_surface_t *surface);
void draw_body(const char *filename, int seeds, int peers, int completed, int file_count, const char *file_size, int tracker_count, int time, cairo_surface_t *surface);
