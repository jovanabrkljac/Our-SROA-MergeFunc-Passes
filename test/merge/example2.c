int inner(int x) {
    int y = x + 2;
    return y * 3;
}

int innerCopy(int x) {
    int y = x + 2;
    return y * 3;
}

int outer(int z) {
    return inner(z) + innerCopy(z);
}

int main() {
    return outer(4);
}
