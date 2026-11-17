# Auto Modellista — Ally companion

Version **0.1.1**. Linux build and launch helpers for a user-supplied **PS2 USA SLUS-20642** disc on ROG Ally/Bazzite.

Uses [ps2-native-recomp](https://github.com/exoteric33/ps2-native-recomp) revision `15a50b77a526f779befa7addb820478485947bdc`. The PS2 main CPU code is translated to native code; the runtime implements the remaining console hardware. This is not a complete game-engine rewrite. The resulting Linux executable uses Vulkan, without Proton.

## Features

- Fullscreen 16:9 with a gameplay field-of-view correction from nemesis2000, maintained by PCSX2.
- 2× internal rendering; this does not mean all original assets render at native 1080p.
- Left stick steering, RT pressure-sensitive Cross/throttle, LT pressure-sensitive Square/brake, A confirm, B back and Menu/Start for the game pause.
- Progressive steering: 8% dead zone, exponent 1.9, 55 ms smoothing. About 23% steering at half stick travel; full steering remains available at the endpoints. The standalone C++ test covers center, symmetry, monotonicity, frame-rate independence, reversal and neutral return.
- Select, then Start (View, then Menu on the Ally) opens the upstream pause and quick-save menu.
- Persistent native memory-card saves and compilation caches.

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

Tested on an Ally Z1 Extreme: startup, formatted memory card, race display near 60 FPS and physical controls confirmed by the user. The whole game and every track have not been tested. First visits to new scenes can pause briefly for compilation; results are cached. Back up `saves/`, especially `memorycard.ps2`, and use the game's save operation. No forced career autosave is added. Quick states from a different executable/patch configuration are not a supported migration method; keep the native memory card instead.

## Distribution

Bring your own game. This repository includes only companion scripts and documentation. Do not redistribute disc images, extracted files, generated game C++, built game executables, saves or game artwork. Helper scripts are MIT licensed; upstream components retain their own licenses. This project is not affiliated with the game publisher.
