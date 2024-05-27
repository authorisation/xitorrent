#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <openssl/sha.h>

#include "torrent_parser.h"

void decode(char** data) {
    switch (**data) {
        case 'd':
            decode_dict(data, 0, NULL, NULL);
            break;
        case 'l':
            decode_list(data);
            break;
        case 'i':
            decode_int(data);
            break;
        default:
            if (isdigit(**data)) {
                decode_string(data);
            } else {
                fprintf(stderr, "Invalid bencoded data\n");
                exit(1);
            }
            break;
    }
}

void decode_dict(char** data, int calculate_info_hash, const char** info_start, long* info_length) {
    (*data)++;
    while (**data != 'e') {
        if (**data == '\0') {
            fprintf(stderr, "Unexpected end of data in dictionary\n");
            exit(1);
        }
        if (calculate_info_hash && strncmp(*data, "4:info", 6) == 0) {
            *data += 6;
            *info_start = *data;
            get_info_hash(data, info_start, info_length);
            break;
        }
        decode_string(data);
        decode(data);
    }
    (*data)++;
}

void decode_list(char** data) {
    (*data)++;
    while (**data != 'e') {
        if (**data == '\0') {
            fprintf(stderr, "Unexpected end of data in list\n");
            exit(1);
        }
        decode(data);
    }
    (*data)++;
}

void decode_int(char** data) {
    (*data)++;
    char* end;
    long int value = strtol(*data, &end, 10);
    if (*data == end || *end != 'e') {
        fprintf(stderr, "Invalid integer in bencoded data\n");
        exit(1);
    }
    *data = end + 1;
}

void decode_string(char** data) {
    char* end;
    long length = strtol(*data, &end, 10);
    if (*data == end || *end != ':' || length < 0) {
        fprintf(stderr, "Invalid string length in bencoded data\n");
        exit(1);
    }
    *data = end + 1;
    *data += length;
}

char* read_file(const char* filename, long* length) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    *length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = (char*)malloc(*length + 1);
    if (!buffer) {
        perror("Failed to alloc mem");
        exit(EXIT_FAILURE);
    }

    fread(buffer, 1, *length, file);
    buffer[*length] = '\0';

    fclose(file);
    return buffer;
}

void get_info_hash(char** data, const char** info_start, long* info_length) {
    char* dict_start = *data;
    decode_dict(data, 0, info_start, info_length);
    *info_length = *data - dict_start;

    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1((unsigned char*)*info_start, *info_length, hash);

    printf("Info hash: ");
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
        printf("%02x", hash[i]);
    }
    printf("\n");
}