"""Connect the SLUS-20642 profile and dashboard to the pinned runtime."""
from pathlib import Path
import shutil
import sys

root = Path(sys.argv[1]).resolve()
source = Path(__file__).resolve().parent

def insert(path, changes):
    text = path.read_text()
    for original, updated in changes:
        if updated in text:
            continue
        if text.count(original) != 1:
            raise ValueError(f"Unexpected source in {path}: {original}")
        text = text.replace(original, updated)
    if text != path.read_text():
        path.write_text(text)

if len(sys.argv) > 2:
    generated = Path(sys.argv[2])
    unit, = [p for p in generated.glob("unit_*.cpp") if "L_00269640:" in p.read_text()]
    insert(unit, [
        ('#include "units.hpp"', '#include "units.hpp"\n#include "../../host/ally_game.hpp"'),
        ("L_00269640:", "L_00269640:\n    ally_game::before_load();"),
        ("L_00269764:", "L_00269764:\n    ally_game::after_load();"),
        ("L_00269630:", "L_00269630:\n    ally_game::after_pack();"),
    ])
else:
    shutil.copyfile(source / "ally_game.hpp", root / "host/ally_game.hpp")
    insert(root / "host/main.cpp", [
        ('#include "crash.hpp"', '#include "crash.hpp"\n#include "ally_game.hpp"'),
        ("ps2::boot(entry, args);", """ps2::boot(entry, args);
        auto present = ps2::g_options.on_vblank;
        ps2::g_options.on_vblank = [present] { ally_game::frame(); if (present) present(); };"""),
        ("const int result = ps2::run();\n        ps2::flush_memory_card();",
         "const int result = ps2::run();\n        ally_game::checkpoint();\n        ps2::flush_memory_card();"),
    ])
    insert(root / "host/window.cpp", [
        ('#include "ally_steering.hpp"', '#include "ally_steering.hpp"\n#include "ally_game.hpp"'),
        ("32767.0), dt);", "32767.0), dt, ally_game::road_speed.load(std::memory_order_relaxed));"),
    ])
    insert(root / "scripts/port.sh", [
        ("build_native() {", 'build_native() {\n  python3 "$ROOT/../../source/apply-profile.py" "$ROOT" "$generated"'),
    ])
