#include <stdio.h>

struct Inner {
    int x;
    int y;
};

struct Outer {
    struct Inner in;
    int z;
};

int nested(int a, int b, int c) {
    struct Outer o;
    o.in.x = a;
    o.in.y = b;
    o.z = c;
    return o.in.x + o.in.y + o.z;
}
