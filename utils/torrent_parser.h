#ifndef TORRENT_PARSER_H
#define TORRENT_PARSER_H

#define INFOHASH_LENGTH 40

void decode(char** data);
void decode_dict(char** data, int calculate_info_hash, const char** info_start, long* info_length);
void decode_list(char** data);
void decode_int(char** data);
void decode_string(char** data);
void get_info_hash(char** data, const char** info_start, long* info_length);
char* read_file(const char* filename, long* length);

#endif
