#include <stdio.h>

int main(void) {
    int x = 5;
    int *p = &x;
    printf("outer x = %d\n", x);
    {
        int x = 99;
        printf("inner x = %d\n", x);
        printf("inner &x = %p\n", (void *) &x);
        printf("p = %p\n", (void *) p);
        printf("*p = %d\n", *p);
    }
}