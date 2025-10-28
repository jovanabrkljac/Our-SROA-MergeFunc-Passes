#include <stdio.h>

int mul_add1(int a, int b) {
    int x = a * 2;
    int y = b * 3;
    int z = x + y;
    return z;
}

int mul_add2(int a, int b) {
    int x = a * 2;
    int y = b * 3;
    int z = x + y;
    return z;
}

int mul_sub1(int a, int b) {
    int x = a * 2;
    int y = b * 3;
    int z = x - y;
    return z;
}

int mul_sub2(int a, int b) {
    int x = a * 2;
    int y = b * 3;
    int z = x - y;
    return z;
}

int with_loop1(int n) {
    int sum = 0;
    for (int i = 0; i < n; i++) {
        sum += i * 2;
    }
    return sum;
}

int with_loop2(int n) {
    int sum = 0;
    for (int i = 0; i < n; i++) {
        sum += i * 2;
    }
    return sum;
}

int branch1(int x) {
    if (x > 0)
        return x * 2;
    else
        return -x;
}

int branch2(int x) {
    if (x > 0)
        return x * 2;
    else
        return -x;
}

int main() {
    printf("%d\n", mul_add1(1,2));
    printf("%d\n", mul_sub1(5,3));
    printf("%d\n", with_loop1(10));
    printf("%d\n", branch1(7));
    return 0;
}