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

int looptest(int n) {
    struct Pair p;
    p.a = 0;
    p.b = 1;

    for (int i = 0; i < n; i++) {
        p.a += i;
        p.b += p.a;
    }

    return p.a + p.b;
}
