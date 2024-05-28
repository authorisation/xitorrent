#include <iostream>
#include <fstream>
#include <sstream>
#include <libtorrent/session.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/magnet_uri.hpp>
#include <libtorrent/torrent_status.hpp>
#include "torrent_parser.h"

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

void parseTorrent(const char* filename, TorrentInfo* torrentInfo) {
    libtorrent::error_code ec;
    libtorrent::torrent_info ti(filename, ec);
    if (ec) {
        return;
    }

    libtorrent::settings_pack settings;
    settings.set_bool(libtorrent::settings_pack::enable_dht, false);
    settings.set_bool(libtorrent::settings_pack::enable_lsd, false);
    settings.set_bool(libtorrent::settings_pack::enable_upnp, false);
    libtorrent::session ses(settings);

    libtorrent::add_torrent_params atp;
    atp.ti = std::make_shared<libtorrent::torrent_info>(ti);
    atp.save_path = ".";
    atp.flags |= libtorrent::torrent_flags::seed_mode;
    libtorrent::torrent_handle th = ses.add_torrent(atp, ec);

    if (ec) {
        return;
    }

    libtorrent::torrent_status status;
    do {
        status = th.status();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    } while (!status.has_metadata);

    for (int i = 0; i < 5; ++i) {
        status = th.status();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    std::string name = status.name;
    std::string infoHash = toHex(status.info_hashes.v1);
    std::string totalSize = formatSize(th.torrent_file()->total_size());

    strncpy(torrentInfo->name, name.c_str(), sizeof(torrentInfo->name));
    strncpy(torrentInfo->info_hash, infoHash.c_str(), sizeof(torrentInfo->info_hash));
    strncpy(torrentInfo->total_size, totalSize.c_str(), sizeof(torrentInfo->total_size));
    torrentInfo->file_count = th.torrent_file()->num_files();
    torrentInfo->num_seeds = status.num_complete;
    torrentInfo->num_peers = status.num_incomplete;
    torrentInfo->num_trackers = th.torrent_file()->trackers().size();
}
