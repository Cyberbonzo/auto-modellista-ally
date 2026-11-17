#!/usr/bin/env bash
set -euo pipefail
root="${AUTOMODELLISTA_ROOT:-$HOME/Games/AutoModellista}"
image=localhost/automodellista-builder:0.1.0
if ! podman image exists "$image"; then
  base="${AM_BUILD_BASE:-registry.fedoraproject.org/fedora:45}"
  podman build --build-arg "BUILD_BASE=$base" -t "$image" -f "$root/source/Containerfile" "$root/source"
fi
python3 "$root/source/apply-controls.py" "$root/upstream/ps2-native-recomp"
python3 "$root/source/apply-profile.py" "$root/upstream/ps2-native-recomp"
find "$root/upstream/ps2-native-recomp/scripts" -maxdepth 1 -type f -name '*.sh' -exec chmod +x {} +
podman run --rm --pull=never --userns=keep-id --security-opt label=disable -e PS2_JOBS=4 -v "$root:$root" -w "$root/upstream/ps2-native-recomp" localhost/automodellista-builder:0.1.0 bash scripts/port.sh automodellista "$root/game/disc.iso" --native-only --no-explore --pnach "$root/docs/widescreen.pnach" --pnach-section "Widescreen 16:9"
cp "$root/upstream/ps2-native-recomp/games/automodellista/patched/SLUS_206.42" "$root/game/boot/SLUS_206.42.wide"
cp "$root/upstream/ps2-native-recomp/games/automodellista/patches.txt" "$root/game/patches.txt"
cp "$root/upstream/ps2-native-recomp/build/native-automodellista/ps2run" "$root/game/AutoModellista.next"
chmod +x "$root/game/AutoModellista.next" "$root/jogar.sh" "$root/source/live-compiler.sh"
mv "$root/game/AutoModellista.next" "$root/game/AutoModellista"
