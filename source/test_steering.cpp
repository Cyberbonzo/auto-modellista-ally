#include "ally_steering.hpp"
#include <cassert>
#include <iostream>
int main() {
    using ally::Steering;
    assert(Steering::curve(0.08)==0 && Steering::curve(-0.08)==0);
    assert(Steering::curve(0.5)>0.18 && Steering::curve(0.5)<0.20);
    assert(Steering::curve(1)==1 && Steering::curve(-1)==-1);
    double last=-1;
    for(int i=-1000;i<=1000;++i){double x=i/1000.0,y=Steering::curve(x);assert(y>=last);assert(std::abs(y+Steering::curve(-x))<1e-12);last=y;}
    assert(std::abs(Steering::curve(1, 160) - 0.72) < 1e-9);
    assert(Steering::curve(1, 30) == 1);
    assert(Steering::curve(0.5, 160) < Steering::curve(0.5, 70));
    Steering a,b;
    for(int i=0;i<30;++i)a.update(1,1.0/60);
    for(int i=0;i<60;++i)b.update(1,1.0/120);
    assert(std::abs(a.value-b.value)<1e-10);
    for (int i=0;i<60;++i) a.update(1,1.0/60);
    assert(a.axis(1,1.0/60)==255);
    assert(a.update(-1,1.0/60)>0); // Reversal does not jump instantly across the center.
    for(int i=0;i<120;++i)a.update(-1,1.0/60);
    assert(a.axis(-1,1.0/60)==0);
    for(int i=0;i<120;++i)a.update(0,1.0/60);
    assert(a.axis(0,1.0/60)==128);
    std::cout<<"PASS: center, monotonicity/symmetry, full travel, frame-rate independence, smooth reversal and neutral return\n";
}
