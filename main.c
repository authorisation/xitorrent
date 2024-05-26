#include </usr/include/cairo/cairo.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <bits/getopt_core.h>
#include <string.h>

#define VERSION "1.0"

void print_help(const char *progname) {
    printf("Usage: [-h] -t text (-b) (-i) -o output.png\n");
    printf("  -h                            Display this help message\n");
    printf("  -t <text>                     Torrent title\n");
    printf("  -i <input.torrent>            Input torrent file\n");
    printf("  -b <seeds;peers;filesize>     Torrent info - Optional, overrides input torrent file.\n");
    printf("  -o <output.png>               Output image file\n");
    printf("  -v                            Display Version\n");
}

cairo_surface_t* init_image(int width, int height) {
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
    cairo_t *cr = cairo_create(surface);

    // Background color
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_paint(cr);

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

    int opt;
    while ((opt = getopt(argc, argv, "ht:b:i:o:v")) != -1) {
        switch (opt) {
            case 'h':
                print_help(argv[0]);
                exit(EXIT_SUCCESS);
            case 't':
                text = optarg;
                break;
            case 'b':
                info = optarg;
                break;
            case 'i':
                input_filename = optarg;
                break;
            case 'o':
                output_filename = optarg;
                break;
            case 'v':
                printf("%s version %s\n", argv[0], VERSION);
                exit(EXIT_SUCCESS);
            default:
                print_help(argv[0]);
                exit(EXIT_FAILURE);
        }
    };

    if (!text || !output_filename || (!info && !input_filename)) {
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