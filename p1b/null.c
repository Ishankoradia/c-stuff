#include <stdio.h>

int main(void) {
    int *p = NULL;
    printf("null ptr = %p\n", (void *)p);

    printf("deref null ptr = %d", *p);
}