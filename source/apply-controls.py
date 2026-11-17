"""Apply the Ally trigger layout to a private checkout of the upstream runtime."""
from pathlib import Path
import sys
p=Path(sys.argv[1])/'host/window.cpp'
t=p.read_text()
pairs={
 'trigger(SDL_CONTROLLER_AXIS_TRIGGERLEFT, kL2, l2);':'trigger(SDL_CONTROLLER_AXIS_TRIGGERLEFT, kSquare, l2);',
 'trigger(SDL_CONTROLLER_AXIS_TRIGGERRIGHT, kR2, r2);':'trigger(SDL_CONTROLLER_AXIS_TRIGGERRIGHT, kCross, r2);',
 'ps2::set_pad_pressure(8, l2, port);':'ps2::set_pad_pressure(15, l2, port);',
 'ps2::set_pad_pressure(9, r2, port);':'ps2::set_pad_pressure(14, r2, port);',
}
for old,new in pairs.items():
    if new in t:continue
    assert t.count(old)==1,old
    t=t.replace(old,new)
p.write_text(t)
print('RT -> pressure-sensitive Cross; LT -> pressure-sensitive Square')
