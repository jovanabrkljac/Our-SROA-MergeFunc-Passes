struct Pair {
    int a;
    int b;
};

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
