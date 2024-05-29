#include <cairo/cairo.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#include "draw.h"
#include "utils/torrent_parser.h"
#include "utils/op.h"

#define VERSION "1.0.0"

int verbose = 0;

void print_help(const char *progname) {
    printf("Usage: %s [-h] [-b] [-t title] -i file.torrent -o output.png\n", progname);
    printf("  -h                            Display this help message\n");
    printf("  -t <text>                     Torrent title\n");
    printf("  -i <input.torrent>            Input torrent file\n");
    printf("  -o <output.png>               Output image file\n");
    printf("  -b                            Verbose output\n");
    printf("  -v                            Display Version\n");
}

int main(int argc, char *argv[]) {
    const char *title = NULL;
    const char *input_filename = NULL;
    const char *output_filename = NULL;

    int opt;
    while ((opt = getopt(argc, argv, "ht:i:o:bv")) != -1) {
        switch (opt) {
            case 'h':
                print_help(argv[0]);
                exit(EXIT_SUCCESS);
            case 't':
                title = optarg;
                break;
            case 'i':
                input_filename = optarg;
                break;
            case 'o':
                output_filename = optarg;
                break;
            case 'b':
                verbose = 1;
                break;
            case 'v':
                printf("version %s\n", VERSION);
                exit(EXIT_SUCCESS);
            default:
                print_help(argv[0]);
                exit(EXIT_FAILURE);
        }
    };

    if (!input_filename || !output_filename) {
        fprintf(stderr, "Error: missing required options\n");
        print_help(argv[0]);
        exit(EXIT_FAILURE);
    }

    //Torrent parser
    TorrentInfo torrent_info;
    parseTorrent(input_filename, &torrent_info);

    verbose_log("Input file: %s\n", input_filename);
    verbose_log("Output file: %s\n", output_filename);

    //Image drawing
    cairo_surface_t *surface = init_image(494, 55);
    title = title ? title : torrent_info.name;
    int unix_time = (int)time(NULL);
    draw_title(output_filename, title, surface);
    draw_body(output_filename, torrent_info.num_seeds, torrent_info.num_peers, 0, torrent_info.file_count, torrent_info.total_size, torrent_info.num_trackers, unix_time, surface);

    verbose_log("Title: %s\n", title);
    verbose_log("Seeders: %d\n", torrent_info.num_seeds);
    verbose_log("Peers: %d\n", torrent_info.num_peers);
    verbose_log("File count: %d\n", torrent_info.file_count);
    verbose_log("Total size: %s\n", torrent_info.total_size);
    verbose_log("Tracker count: %d\n", torrent_info.num_trackers);
    verbose_log("Time: %d\n", unix_time);
    return 0;
}
