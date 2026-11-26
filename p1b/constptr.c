#include <stdio.h>

int main(void) {
    char c = 'a';
    char c2 = 'k';

    // const char *a = &c;
    // a = &c2;
    // *a = 'x';

    // char *const b = &c;
    // *b = 'x';
    // b = &c2;

    const char * const d = &c;
    *d = 'x';
    d = &c2;

}