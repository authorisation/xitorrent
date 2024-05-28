#include <cairo/cairo.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <bits/getopt_core.h>
#include <string.h>

#include "draw.h"
#include "utils/torrent_parser.h"

#define VERSION "1.0.0"

void print_help(const char *progname) {
    printf("Usage: [-h] -t text -i file.torrent -o output.png\n");
    printf("  -h                            Display this help message\n");
    printf("  -t <text>                     Torrent title\n");
    printf("  -i <input.torrent>            Input torrent file\n");
    printf("  -o <output.png>               Output image file\n");
    printf("  -v                            Display Version\n");
}

int main(int argc, char *argv[]) {
    const char *text = NULL;
    const char *info = NULL;
    const char *input_filename = NULL;
    const char *output_filename = NULL;

    int opt;
    while ((opt = getopt(argc, argv, "ht:i:o:v")) != -1) {
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

    if (!text || !input_filename || !output_filename) {
        fprintf(stderr, "Error: missing required options\n");
        print_help(argv[0]);
        exit(EXIT_FAILURE);
    }

    int width = 494;
    int height = 55;
    cairo_surface_t *surface = init_image(width, height);

    TorrentInfo torrent_info;
    parseTorrent(input_filename, &torrent_info);

    draw_title(output_filename, text, info, surface);
    draw_body(output_filename, torrent_info.num_seeds, torrent_info.num_peers, 0, torrent_info.file_count, torrent_info.total_size, torrent_info.num_trackers, 0, surface);

    return 0;
}
