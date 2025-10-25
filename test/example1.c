struct Pair {
    int x;
    int y;
};

void foo() {
    struct Pair p;
    p.x = 10;
    p.y = 20;
}

void bar() {
    int a[5];
    a[0] = 1;
    a[1] = 2;
}
