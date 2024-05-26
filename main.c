#include </usr/include/cairo/cairo.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <bits/getopt_core.h>
#include <string.h>

#define VERSION "1.0.0"

void print_help(const char *progname) {
    printf("Usage: [-h] -t text -i file.torrent -x trackers.txt -o output.png\n");
    printf("  -h                            Display this help message\n");
    printf("  -t <text>                     Torrent title\n");
    printf("  -i <input.torrent>            Input torrent file\n");
    printf("  -x <trackers.txt>             Tracker list to scan against\n");
    printf("  -o <output.png>               Output image file\n");
    printf("  -v                            Display Version\n");
}

cairo_surface_t* init_image(int width, int height) {
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
    cairo_t *cr = cairo_create(surface);

    // Background color
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

    // Text options
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_select_font_face(cr, "Tahoma", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 14);

    // Draw the torrent title
    cairo_move_to(cr, 5, 17);
    cairo_show_text(cr, text);

    cairo_surface_write_to_png(surface, filename);

    cairo_destroy(cr);
    cairo_surface_destroy(surface);
}

int main(int argc, char *argv[]) {
    const char *text = NULL;
    const char *info = NULL;
    const char *input_filename = NULL;
    const char *output_filename = NULL;
    const char *trackers_filename = NULL;

    int opt;
    while ((opt = getopt(argc, argv, "ht:i:x:o:v")) != -1) {
        switch (opt) {
            case 'h':
                print_help(argv[0]);
                exit(EXIT_SUCCESS);
            case 't':
                text = optarg;
                break;
            case 'i':
                input_filename = optarg;
                break;
            case 'x':
                trackers_filename = optarg;
                break;
            case 'o':
                output_filename = optarg;
                break;
            case 'v':
                printf("version %s\n", VERSION);
                exit(EXIT_SUCCESS);
            default:
                print_help(argv[0]);
                exit(EXIT_FAILURE);
        }
    };

    if (!text || !input_filename || !trackers_filename || !output_filename) {
        fprintf(stderr, "Error: missing required options\n");
        print_help(argv[0]);
        exit(EXIT_FAILURE);
    }

    int width = 494;
    int height = 55;
    cairo_surface_t *surface = init_image(width, height);

    draw_title(output_filename, text, info, surface);
    return 0;
}