#include <stdio.h>

struct Stats {
    int hp;
    int atk;
    int def;
    int spd;
    int luck;
};

int initA() {
    struct Stats s;
    s.hp = 100;
    s.atk = 20;
    s.def = 10;
    s.spd = 5;
    s.luck = 3;

    int total = s.hp + s.atk + s.def + s.spd + s.luck;
    return total;
}

int initB() {
    struct Stats s;
    s.hp = 100;
    s.atk = 20;
    s.def = 10;
    s.spd = 5;
    s.luck = 3;

    int total = s.hp + s.atk + s.def + s.spd + s.luck;
    return total;
}

int boostA() {
    struct Stats s;
    s.hp = 80;
    s.atk = 25;
    s.def = 12;
    s.spd = 7;
    s.luck = 4;

    s.hp += 5;
    s.atk += 2;
    s.luck += 1;

    return s.hp + s.atk + s.def + s.spd + s.luck;
}

int boostB() {
    struct Stats s;
    s.hp = 80;
    s.atk = 25;
    s.def = 12;
    s.spd = 7;
    s.luck = 4;

    s.hp += 5;
    s.atk += 2;
    s.luck += 1;

    return s.hp + s.atk + s.def + s.spd + s.luck;
}

int boostC() {
    struct Stats s;
    s.hp = 80;
    s.atk = 25;
    s.def = 12;
    s.spd = 7;
    s.luck = 4;

    // mali varijantni kod — ne bi trebalo da se spoji
    s.hp += 10;
    s.atk += 1;
    s.luck -= 1;

    return s.hp + s.atk + s.def + s.spd + s.luck;
}

int main() {
    int t1 = initA();
    int t2 = initB();
    int b1 = boostA();
    int b2 = boostB();
    int b3 = boostC();

    printf("Init sum: %d\n", t1 + t2);
    printf("Boost sum: %d\n", b1 + b2 + b3);

    return t1 + t2 + b1 + b2 + b3;
}
