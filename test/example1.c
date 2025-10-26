#include <stdio.h>

struct Pair {
    int x;
    int y;
};

int update(int x, int y) {
    struct Pair p;
    p.x = x;
    p.y = y;
    return p.x + p.y;
}
