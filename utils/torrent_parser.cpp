#include <iostream>
#include <fstream>
#include <sstream>
#include <libtorrent/session.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/magnet_uri.hpp>
#include <libtorrent/torrent_status.hpp>

#include "torrent_parser.h"
#include "op.h"

std::string formatSize(int64_t size) {
    const char* units[] = {"B", "KB", "MB", "GB"};
    int unitIndex = 0;
    double readableSize = static_cast<double>(size);

    while (readableSize >= 1024 && unitIndex < 3) {
        readableSize /= 1024;
        ++unitIndex;
    }

    char buffer[50];
    std::snprintf(buffer, sizeof(buffer), "%.2f %s", readableSize, units[unitIndex]);
    return std::string(buffer);
}

std::string toHex(const libtorrent::sha1_hash& hash) {
    std::stringstream ss;
    ss << hash;
    return ss.str();
}

char* readFile(const char* filename, long* length) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file) {
        return nullptr;
    }

    *length = file.tellg();
    file.seekg(0, std::ios::beg);

    char* buffer = new char[*length + 1];
    if (!buffer) {
        return nullptr;
    }

    file.read(buffer, *length);
    buffer[*length] = '\0';

    file.close();
    return buffer;
}

bool torrent_health_check(const char* filename) {
    struct stat statbuf;
    if (stat(filename, &statbuf) != 0) {
        printf("Failed to stat file: %s\n", filename);
        return false;
    }
    if (S_ISDIR(statbuf.st_mode)) {
        printf("Error: %s is a directory\n", filename);
        return false;
    }

    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs) {
        printf("Failed to open file: %s\n", filename);
        return false;
    }

    std::vector<char> buffer((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    ifs.close();

    libtorrent::error_code ec;
    libtorrent::bdecode_node node;
    libtorrent::bdecode(buffer.data(), buffer.data() + buffer.size(), node, ec);
    if (ec) {
        printf("Failed bdecode: %s\n", filename);
        return false;
    }

    return true;
}

void parseTorrent(const char* filename, TorrentInfo* torrentInfo) {
    if (!torrent_health_check(filename)) {
        printf("Error: invalid torrent file\n");
        exit(EXIT_FAILURE);
    }

    libtorrent::error_code ec;
    libtorrent::torrent_info ti(filename, ec);
    if (ec) {
        return;
    }
    verbose_log("Torrent file loaded successfully\n");

    libtorrent::settings_pack settings;
    settings.set_bool(libtorrent::settings_pack::enable_dht, true);
    settings.set_bool(libtorrent::settings_pack::enable_lsd, true);
    settings.set_bool(libtorrent::settings_pack::enable_upnp, true);
    settings.set_bool(libtorrent::settings_pack::enable_natpmp, true);
    libtorrent::session ses(settings);
    verbose_log("Session created\n");

    libtorrent::add_torrent_params atp;
    atp.ti = std::make_shared<libtorrent::torrent_info>(ti);
    atp.save_path = ".";
    atp.flags |= libtorrent::torrent_flags::upload_mode;
    libtorrent::torrent_handle th = ses.add_torrent(atp, ec);
    if (ec) {
        return;
    }
    verbose_log("Torrent added to session successfully\n");

    auto trackers = th.torrent_file()->trackers();
    for (auto& tracker : trackers) {
        th.add_tracker(libtorrent::announce_entry(tracker.url));
    }
    verbose_log("Trackers added\n");

    libtorrent::torrent_status status;
    while (!status.has_metadata) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        status = th.status();
    }
    verbose_log("Metadata acquired\n");

    std::atomic<bool> done(false);
    std::thread([&]() {
        for (int i = 0; i < 6 && !done; ++i) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            status = th.status();
            verbose_log("Status update %d: seeds=%d, peers=%d, download_rate=%.2f kB/s\n",
                i, status.num_complete, status.num_incomplete, status.download_rate / 1000.0);
        }
        done = true;
    }).detach();

    while (!done) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::string name = status.name;
    std::string infoHash = toHex(status.info_hashes.v1);
    std::string totalSize = formatSize(th.torrent_file()->total_size());

    verbose_log("Torrent name: %s\n", name.c_str());
    verbose_log("Info hash: %s\n", infoHash.c_str());
    verbose_log("Total size: %s\n", totalSize.c_str());

    strncpy(torrentInfo->name, name.c_str(), sizeof(torrentInfo->name));
    strncpy(torrentInfo->info_hash, infoHash.c_str(), sizeof(torrentInfo->info_hash));
    strncpy(torrentInfo->total_size, totalSize.c_str(), sizeof(torrentInfo->total_size));
    torrentInfo->file_count = th.torrent_file()->num_files();
    torrentInfo->num_seeds = status.num_complete;
    torrentInfo->num_peers = status.num_incomplete;
    torrentInfo->num_trackers = th.torrent_file()->trackers().size();

    verbose_log("Torrent info parsed: file_count=%d, num_seeds=%d, num_peers=%d, num_trackers=%d\n",
            torrentInfo->file_count, torrentInfo->num_seeds, torrentInfo->num_peers, torrentInfo->num_trackers);
}