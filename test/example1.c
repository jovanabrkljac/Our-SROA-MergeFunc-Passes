struct Pair {
    int x;
    int y;
};

void foo() {
    struct Pair p;
    p.x = 10;
    p.y = 20;
    int a = p.x;
}
