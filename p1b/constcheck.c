#include <stdio.h>

int main(void) {
    const int x = 5;
    printf("before x = %d\n", x);
    printf("before &x = %p\n",&x);
    int *p = (int *) &x;
    *p = 99;
    printf("after x = %d\n", x);
    printf("*p = %p\n", p);
    printf("&x = %p\n",&x);
}