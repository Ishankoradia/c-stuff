#include <stdio.h>

int main(void) {
    int x = 5;
    int *p = &x;

    printf("&x = %p\n", (void *)&x);
    printf("p  = %p\n", (void *)p);
    printf("*p = %d\n", *p);

    *p = 99;

    printf("x = %d\n", x);

    printf("ptr size int = %zu and ptr size char = %zu\n", sizeof(int *), sizeof(char *));
}