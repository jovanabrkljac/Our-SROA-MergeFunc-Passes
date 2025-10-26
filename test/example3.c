struct Pair {
    int a;
    int b;
    int c;
};

int update(int x, int y) {
    struct Pair p;
    p.a = x + y;
    p.b = x - y;

    int t1 = p.a;
    int t2 = p.b;

    p.a = t1 * 2;
    p.b = t2 * 3;

    if (x > 0)
        p.a = p.a + 1;
    else
        p.b = p.b + 1;

    return p.a + p.b;
}
