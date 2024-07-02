#include <cairo/cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/op.h"
#include "draw.h"

const AssetMapping asset_mappings[] = {
    {"ai1", "ai_asset_1.png", 0.01425},
    {"ai2", "ai_asset_2.png", 0.08},
    {"ai3", "ai_asset_3.png", 0.1035},
    {"yumeko", "yumeko_asset.png", 0.155555}
};

const int num_assets = sizeof(asset_mappings) / sizeof(asset_mappings[0]);

const char* get_asset_filename(const char *option) {
    for (int i = 0; i < num_assets; i++) {
        if (strcmp(asset_mappings[i].option, option) == 0) {
            verbose_log("Got asset filename: %s\n", asset_mappings[i].filename);
            return asset_mappings[i].filename;
        }
    }
    return "yumeko_asset.png";
}

double get_scale_factor(const char *option) {
    for (int i = 0; i < num_assets; i++) {
        if (strcmp(asset_mappings[i].option, option) == 0) {
            return asset_mappings[i].scale_factor;
        }
    }
    return 0.155555;
}

cairo_surface_t* init_image(int width, int height, const char *asset) {
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
    cairo_t *cr = cairo_create(surface);

    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_paint(cr);

    const char* final_asset = get_asset_filename(asset);
    const char *goddess = final_asset;

    cairo_surface_t *image = cairo_image_surface_create_from_png(goddess);
    if (cairo_surface_status(image) != CAIRO_STATUS_SUCCESS) {
        fprintf(stderr, "Error: Could not load image %s\n", goddess);
        cairo_destroy(cr);
        cairo_surface_destroy(surface);
        return NULL;
    }

    int image_width = cairo_image_surface_get_width(image);
    int image_height = cairo_image_surface_get_height(image);
    double scale_factor = get_scale_factor(asset);
    double scaled_width = image_width * scale_factor;
    double scaled_height = image_height * scale_factor;

    cairo_save(cr);
    cairo_translate(cr, width - scaled_width - 10, height - scaled_height);
    cairo_scale(cr, scale_factor, scale_factor);
    cairo_set_source_surface(cr, image, 0, 0);
    cairo_paint(cr);
    cairo_restore(cr);

    cairo_surface_destroy(image);
    cairo_destroy(cr);
    return surface;
}

void draw_title(const char *filename, const char *text, cairo_surface_t *surface) {
    cairo_t *cr = cairo_create(surface);
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_select_font_face(cr, "Tahoma", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 14);

    cairo_move_to(cr, 5, 17);
    cairo_show_text(cr, text);

    cairo_surface_write_to_png(surface, filename);

    cairo_destroy(cr);
}

void draw_body(const char *filename, int seeds, int peers, int completed, int file_count, const char *file_size, int tracker_count, int time, cairo_surface_t *surface) {
    cairo_t *cr = cairo_create(surface);

    // Text options
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_select_font_face(cr, "Tahoma", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 14);

    cairo_move_to(cr, 5, 32);
    char info_text[256];
    snprintf(info_text, sizeof(info_text), "files: %d (%s); seeds: ", file_count, file_size);
    cairo_show_text(cr, info_text);

    cairo_set_source_rgb(cr, 0, .8, 0);
    snprintf(info_text, sizeof(info_text), "%d", seeds);
    cairo_show_text(cr, info_text);

    cairo_set_source_rgb(cr, 0, 0, 0);
    snprintf(info_text, sizeof(info_text), "; leeches: ");
    cairo_show_text(cr, info_text);

    cairo_set_source_rgb(cr, .8, 0, 0);
    snprintf(info_text, sizeof(info_text), "%d", peers);
    cairo_show_text(cr, info_text);

    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_move_to(cr, 5, 47);
    char tracker_text[256];
    snprintf(tracker_text, sizeof(tracker_text), "info from %d trackers - time: %d", tracker_count, time);
    cairo_show_text(cr, tracker_text);

    cairo_surface_write_to_png(surface, filename);

    cairo_destroy(cr);
    //Operation is considered done, so destroy the surface
    cairo_surface_destroy(surface);
}
