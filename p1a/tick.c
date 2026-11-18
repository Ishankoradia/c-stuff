#include <stdio.h>

void counter(void) {
    static int cnt = 0;
    cnt ++;
    printf("cnt = %d\n", cnt);
}

int main(void) {
    counter();
    counter();
    counter();
    counter();
}