#include </usr/include/cairo/cairo.h>
#include <stdio.h>

cairo_surface_t* init_image(int width, int height) {
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
    cairo_t *cr = cairo_create(surface);

    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_paint(cr);

    // Yumeko!
    const char *image_path = "yumeko_asset.png";
    cairo_surface_t *image = cairo_image_surface_create_from_png(image_path);
    if (cairo_surface_status(image) != CAIRO_STATUS_SUCCESS) {
        fprintf(stderr, "Error: Could not load image %s\n", image_path);
        cairo_destroy(cr);
        cairo_surface_destroy(surface);
        return NULL;
    }
    int image_width = cairo_image_surface_get_width(image);
    int image_height = cairo_image_surface_get_height(image);
    double scale_factor = 0.155555;
    double scaled_width = image_width * scale_factor;
    double scaled_height = image_height * scale_factor;
    cairo_save(cr);
    cairo_translate(cr, width - scaled_width - 10, height - scaled_height);
    cairo_scale(cr, scale_factor, scale_factor);
    cairo_set_source_surface(cr, image, 0, 0);
    cairo_paint(cr);
    cairo_restore(cr);

    cairo_destroy(cr);
    return surface;
}


void draw_title(const char *filename, const char *text, const char *info, cairo_surface_t *surface) {
    cairo_t *cr = cairo_create(surface);
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_select_font_face(cr, "Tahoma", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 14);

    cairo_move_to(cr, 5, 17);
    cairo_show_text(cr, text);

    cairo_surface_write_to_png(surface, filename);

    cairo_destroy(cr);
}

void draw_body(const char *filename, int seeds, int peers, int completed, int file_count, double file_size, int tracker_count, int time, cairo_surface_t *surface) {
    cairo_t *cr = cairo_create(surface);

    // Text options
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_select_font_face(cr, "Tahoma", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 14);

    cairo_move_to(cr, 5, 32);
    cairo_show_text(cr, "files : %s (%s); seeds: %s; peers: %s; completed %s");
    cairo_move_to(cr, 5, 47);
    cairo_show_text(cr, "info from %s trackers - %s (%s)");

    cairo_surface_write_to_png(surface, filename);

    cairo_destroy(cr);
    cairo_surface_destroy(surface);
}
