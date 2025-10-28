struct Data {
    int x;
    int y;
};

int calc1() {
    struct Data d;
    d.x = 10;
    d.y = 20;
    int result = d.x + d.y;
    return result * 2;
}

int calc2() {
    struct Data d;
    d.x = 10;
    d.y = 20;
    int result = d.x + d.y;
    return 2 * result;
}

int main() {
    return calc1() + calc2();
}
