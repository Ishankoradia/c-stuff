#include <stdio.h>

size_t str_len(const char *ptr) {
    size_t l = 0;

    while (*ptr != '\0') {
        l = l + 1;
        ptr = ptr + 1;
    }
    return l;
}

int main(void) {
    const char *p = "Hi there";
    printf("size of %s = %zu\n", p, str_len(p));
}