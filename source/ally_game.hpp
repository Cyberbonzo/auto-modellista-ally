#pragma once
#include "ps2/memory.hpp"
#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

namespace ally_game {
// SLUS-20642. These are the same persistent blocks and order used by
// Save_game_data_set and Load_game_data_set. Racing and graphics RAM stay out.
struct Block { unsigned address, length; };
inline constexpr Block blocks[] = {
    {0x503ca0, 0x50}, {0x503c50, 0x48}, {0x502cd0, 0xf78},
    {0x502b90, 0x14}, {0x4f5e90, 0xcd00}, {0x4e2b10, 0x13380},
    {0x4dd170, 0x704}, {0x503f10, 0xaa0}, {0x3b59b0, 0x10},
    {0x4b8240, 0x728}, {0x4b8970, 0x300}
};
inline constexpr size_t profile_size = [] {
    size_t n = 0;
    for (auto b : blocks) n += b.length;
    return n;
}();
static_assert(profile_size == 0x22c80);
using Bytes = std::array<unsigned char, profile_size>;
inline constexpr uint64_t magic = 0x31454c49464f5250ULL;
struct Header { uint64_t format, card, last_save, previous_save, checksum; };
inline Header header{magic, 0, 0, 0, 0};
inline Bytes saved{}, current{};
inline bool ready = false, loading = false, written = false, first_load = true;
inline unsigned ticks = 0;
inline std::atomic<int> road_speed{0};
inline const char* path() { return std::getenv("AM_PROFILE_PATH"); }
inline uint32_t word(unsigned a) {
    uint32_t v;
    std::memcpy(&v, ps2::g_ram+a, sizeof(v));
    return v;
}
inline uint64_t digest(const void* data, size_t n, uint64_t h = 14695981039346656037ULL) {
    const auto p = static_cast<const uint8_t*>(data);
    for (size_t i=0; i<n; ++i) h = (h ^ p[i]) * 1099511628211ULL;
    return h;
}
inline uint64_t signature(const Header& h, const Bytes& data) {
    return digest(data.data(), data.size(), digest(&h, offsetof(Header, checksum)));
}
inline uint8_t* card_buffer() {
    const unsigned a = word(0x54a6b0);
    return a >= 0x100000 && a <= 0x2000000-profile_size ? ps2::g_ram+a : nullptr;
}
// The game has already verified the native card data when it enters its unpacker.
inline void before_load() {
    auto dst = card_buffer();
    if (!path() || !dst) return;
    loading = true;
    const auto id = digest(dst, profile_size);
    Header disk{};
    bool restore = false;
    if (first_load) {
        if (FILE* f = std::fopen(path(), "rb")) {
            restore = std::fread(&disk, sizeof(disk), 1, f) == 1 &&
                disk.format == magic && (id == disk.card || id == disk.last_save || id == disk.previous_save) &&
                std::fread(saved.data(), 1, saved.size(), f) == saved.size() &&
                std::fgetc(f) == EOF && signature(disk, saved) == disk.checksum;
            std::fclose(f);
        }
    }
    if (restore) std::memcpy(dst, saved.data(), saved.size());
    header = {magic, id, 0, 0, 0};
    written = false;
    first_load = false;
    std::fprintf(stderr, "[progress] %s\n", restore ? "automatic profile restored" : "native card loaded");
}
inline void after_load() {
    if (loading) { ready = true; loading = false; }
}
inline void after_pack() {
    auto src = card_buffer();
    if (!path() || !src) return;
    const auto id = digest(src, profile_size);
    if (!ready) { header.card = id; ready = true; }
    if (header.last_save != id) {
        header.previous_save = header.last_save;
        header.last_save = id;
        written = false;
    }
}
inline void checkpoint() {
    if (!ready || loading || !path()) return;
    size_t offset = 0;
    for (auto b : blocks) {
        std::memcpy(current.data()+offset, ps2::g_ram+b.address, b.length);
        offset += b.length;
    }
    // The native packer also copies the live play counter into Garage_Work.
    const auto sys = word(0x3b5a24);
    if (sys == 0x53f190) {
        constexpr size_t garage = 0x50+0x48+0xf78+0x14+0xcd00+0x13380;
        std::memcpy(current.data()+garage+0x650, ps2::g_ram+sys+0x58, 4);
    }
    if (written && current == saved) return;
    header.checksum = signature(header, current);
    const std::string temp = std::string(path()) + ".tmp";
    FILE* f = std::fopen(temp.c_str(), "wb");
    if (!f) { std::perror("[progress] open"); return; }
    bool ok = std::fwrite(&header, sizeof(header), 1, f)==1 &&
        std::fwrite(current.data(), 1, current.size(), f)==current.size();
    if (std::fclose(f) != 0) ok = false;
    if (ok && std::rename(temp.c_str(), path()) == 0) {
        saved = current;
        if (!written) std::fprintf(stderr, "[progress] automatic profile saved\n");
        written = true;
    } else {
        std::perror("[progress] write");
        std::remove(temp.c_str());
    }
}
inline void frame() {
    if (word(0x3b5a24) == 0x53f190) ps2::g_ram[0x53f3ef] = 0; // Native metric HUD.
    const unsigned car = word(0x53f170);
    float kmh10 = 0;
    if (car >= 0x100000 && car < 0x1fff000)
        std::memcpy(&kmh10, ps2::g_ram+car+0x17c, 4);
    road_speed.store(std::isfinite(kmh10) ? int(std::min(std::abs(kmh10)*0.1f, 500.f)) : 0, std::memory_order_relaxed);
    if (++ticks >= 120) { ticks = 0; checkpoint(); }
}
}
