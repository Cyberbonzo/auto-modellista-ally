#!/usr/bin/env bash
set -euo pipefail
root="$(cd -- "$(dirname -- "$0")" && pwd -P)"
cd "$root"
mkdir -p logs saves game/cache
exec 9>logs/session.lock
flock -n 9 || exit 0
export SDL_JOYSTICK_HIDAPI=0
export SDL_JOYSTICK_ALLOW_BACKGROUND_EVENTS=1
export PS2_VU_CXX="$root/source/live-compiler.sh"
export PS2_SCALE=2
export PS2_LOOK=0
export PS2_NO_BEEP=1
export PS2_WINDOW_TITLE="Auto Modellista"
unset LD_PRELOAD
exec "$root/game/AutoModellista" "$root/game/boot/SLUS_206.42.wide" --disc "$root/game/disc.iso" --fullscreen --widescreen --patches "$root/game/patches.txt" --memcard "$root/saves/memorycard.ps2" --quick-dir "$root/saves" --game-dir "$root/upstream/ps2-native-recomp/games/automodellista" "$@" >>logs/session.log 2>&1
