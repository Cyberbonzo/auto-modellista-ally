#include "ally_game.hpp"
#include <cassert>
#include <iostream>
#include <filesystem>
namespace ps2 { uint8_t g_ram[0x2000000]{}; }

// Invoke in a private temporary directory with a new filename.
int main(int argc, char** argv) {
    using namespace ally_game;
    assert(argc == 2 && !std::filesystem::exists(argv[1]));
    setenv("AM_PROFILE_PATH", argv[1], 1);
    constexpr uint32_t buffer = 0x1000000;
    std::memcpy(ps2::g_ram+0x54a6b0, &buffer, sizeof(buffer));
    checkpoint();
    assert(!std::filesystem::exists(argv[1]));
    before_load(); after_load();
    for (auto b : blocks) {
        ps2::g_ram[b.address] = 0x42;
        ps2::g_ram[b.address+b.length-1] = 0x91;
    }
    checkpoint();
    const auto expected = saved;
    first_load = true;
    before_load();
    assert(std::memcmp(card_buffer(), expected.data(), profile_size) == 0);
    after_load();

    // An independently changed card supersedes the automatic copy.
    std::memset(card_buffer(), 0x19, profile_size);
    first_load = true;
    before_load();
    assert(card_buffer()[0] == 0x19 && card_buffer()[profile_size-1] == 0x19);
    after_load(); after_pack(); checkpoint();

    // Damaged or partial files must never replace valid native data.
    const auto f = std::fopen(argv[1], "wb");
    assert(f);
    std::fputc(0, f);
    std::fclose(f);
    first_load = true;
    before_load();
    assert(card_buffer()[0] == 0x19 && card_buffer()[profile_size-1] == 0x19);
    std::cout << "PASS: all eleven data blocks, newer card and interrupted save\n";
}
