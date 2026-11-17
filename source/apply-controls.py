"""Apply the Ally trigger layout to a private checkout of the upstream runtime."""
from pathlib import Path
import sys,shutil
p=Path(sys.argv[1])/'host/window.cpp'
t=p.read_text()
pairs={
 '#include "window.hpp"':'#include "window.hpp"\n#include "ally_steering.hpp"',
 'trigger(SDL_CONTROLLER_AXIS_TRIGGERLEFT, kL2, l2);':'trigger(SDL_CONTROLLER_AXIS_TRIGGERLEFT, kSquare, l2);',
 'trigger(SDL_CONTROLLER_AXIS_TRIGGERRIGHT, kR2, r2);':'trigger(SDL_CONTROLLER_AXIS_TRIGGERRIGHT, kCross, r2);',
 'ps2::set_pad_pressure(8, l2, port);':'ps2::set_pad_pressure(15, l2, port);',
 'ps2::set_pad_pressure(9, r2, port);':'ps2::set_pad_pressure(14, r2, port);',
 'if (lx == 0x80) lx = axis(pad, SDL_CONTROLLER_AXIS_LEFTX);':'''if (lx == 0x80) {
            static ally::Steering steering[2];
            static Uint64 previous[2] = {};
            const Uint64 now_ticks = SDL_GetPerformanceCounter();
            const double dt = previous[port] ? double(now_ticks - previous[port]) / SDL_GetPerformanceFrequency() : 1.0 / 60.0;
            previous[port] = now_ticks;
            const int raw = SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_LEFTX);
            lx = steering[port].axis(double(raw) / (raw < 0 ? 32768.0 : 32767.0), dt);
        }''',
}
for old,new in pairs.items():
    if new in t:continue
    assert t.count(old)==1,old
    t=t.replace(old,new)
p.write_text(t)
shutil.copy2(Path(__file__).with_name('ally_steering.hpp'),p.with_name('ally_steering.hpp'))
print('RT -> Cross, LT -> Square; steering: 8% dead zone, exponent 1.9, 55 ms smoothing')
