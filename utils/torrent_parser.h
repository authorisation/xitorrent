#ifdef __cplusplus
extern "C" {
#endif

#define INFOHASH_LENGTH 40

typedef struct {
    char name[256];
    char info_hash[INFOHASH_LENGTH + 1];
    char total_size[32];
    int file_count;
    int num_seeds;
    int num_peers;
    int num_trackers;
} TorrentInfo;

void parseTorrent(const char* filename, TorrentInfo* torrent_info);
char* readFile(const char* filename, long* length);

#ifdef __cplusplus
}
#endif
