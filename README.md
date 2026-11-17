# Auto Modellista — Ally companion

Version **0.1.2**. Linux build and launch helpers for a user-supplied **PS2 USA SLUS-20642** disc on ROG Ally/Bazzite.

Uses [ps2-native-recomp](https://github.com/exoteric33/ps2-native-recomp) revision `15a50b77a526f779befa7addb820478485947bdc`. The PS2 main CPU code is translated to native code; the runtime implements the remaining console hardware. This is not a complete game-engine rewrite. The resulting Linux executable uses Vulkan, without Proton.

## Features

- Fullscreen 16:9 with a gameplay field-of-view correction from nemesis2000, maintained by PCSX2.
- 3× internal rendering with a sharp presentation filter and sharper HUD/text. The race speedometer uses km/h.
- Left stick steering, RT pressure-sensitive Cross/throttle, LT pressure-sensitive Square/brake, A confirm, B back and Menu/Start for the game pause.
- Progressive steering with a 10% dead zone, 100 ms turn-in and 45 ms unwind. Maximum travel tapers from 100% at 45 km/h to 72% at 150 km/h. The left stick retains its full range for tight, slow corners.
- Select, then Start (View, then Menu on the Ally) opens the upstream pause and quick-save menu.
- Career autosave samples the eleven native profile blocks every 120 frames after loading or creating a valid card profile. The next launch restores matching progress through the game's loader. Back up `saves/memorycard.ps2` and `saves/garage-progress.bin` together. A changed card or damaged snapshot falls back to the native card data.

The [widescreen patch](https://github.com/PCSX2/pcsx2_patches/blob/efd872286d9b2ae77530b82fbc56a3110caad960/patches/SLUS-20642_6D76177B.pnach) declares gameplay support, without separate fixes for menus, videos or replays. The race view was visually checked: car proportions are retained and more scenery is visible at the sides. HUD and menus still widen in this mode. Preparation downloads the pinned patch and checks its Git blob identity. The original disc remains unchanged.

## Build

Requires Linux, Python 3, Git, Podman, SDL2 runtime and Vulkan drivers. Building uses Clang, CMake, Ninja and SDL2 headers inside a container. The script reuses `localhost/automodellista-builder:0.1.0` if available; otherwise it builds the required tool container. `AM_BUILD_BASE` can select an existing suitable Fedora/Clang base. No host OS package installer is invoked.

1. Clone this companion into `~/Games/AutoModellista`.
2. Run `python3 source/prepare.py /path/to/your/disc.iso` (a ZIP containing one ISO is also accepted).
3. Run `bash source/build-linux.sh`. The upstream script obtains its pinned, checksum-verified wgpu-native dependency and generates private C++ from your disc.
4. Add `jogar.sh` to Steam as a native non-Steam shortcut. Close other games before testing.

For another folder set `AUTOMODELLISTA_ROOT` while building. Keep the builder image: the live compiler wrapper reuses its compiler for newly encountered code, without downloading a compiler during play.

Supported original ELF SHA-256: `62d6380687050d425b0e3d5e58a2c54ec23909ef9ed8be816428edc7d1036c80`.

## Validation and limitations

The player confirmed that version 0.1.2 made fast corners more stable with the Ally Z1 Extreme’s built-in stick. Steam relaunch restored the automatic profile. A live race showed km/h and five consecutive samples at 60 FPS with 3× rendering. The standalone tests cover speed-dependent steering, all eleven profile blocks, an independently changed card and a truncated snapshot. Individual unlock conditions and all tracks have yet to be tested. First visits to new scenes may pause for compilation.

## Distribution

Bring your own game. This repository includes only companion scripts and documentation. Do not redistribute disc images, extracted files, generated game C++, built game executables, saves or game artwork. Helper scripts are MIT licensed; upstream components retain their own licenses. This project is not affiliated with the game publisher.
