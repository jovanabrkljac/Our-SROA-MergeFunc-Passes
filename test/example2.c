#include <stdio.h>

struct Pair {
    int a;
    int b;
};

int branchy(int x) {
    struct Pair p;
    p.a = 5;
    p.b = 10;

    if (x > 0)
        p.a += 1;
    else
        p.b -= 1;

    return p.a + p.b;
}
