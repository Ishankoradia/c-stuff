#include <stdio.h>

struct Pair {int x; int y;};

struct Pair ret(int x, int y) {
    struct Pair xy  = {x, y};

    return xy;
}

int main(void) {
    struct Pair f = ret(5, 10);
    printf("x = %d\n", f.x);
    printf("y = %d\n", f.y);
}