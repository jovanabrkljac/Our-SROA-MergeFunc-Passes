struct Data {
    int a;
    int b;
    int c;
};

int test(int x) {
    struct Data d;
    d.a = x + 1;
    d.b = x + 2;
    return d.a;
}
