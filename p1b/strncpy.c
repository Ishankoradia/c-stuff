#include <stdio.h>
#include <string.h>

void my_strncpy(char *dest, const char *src, size_t n) {
    while (*src != '\0' && n > 0) {
        *dest = *src;
        dest = dest + 1;
        src = src + 1;
        n = n - 1;
    }

    while (n > 0) {
        *dest = '\0';
        n = n - 1;
        dest = dest + 1;
    }

}

int main(void) {
    const char *src = "hi there.how are you doing ?";
    char dest[6];
    my_strncpy(dest, src, 5);
    *(dest+5) = '\0';

    printf("src @ %p\n", src);
    printf("src = '%s'\n", src);
    printf("dest @ %p\n", dest);
    printf("dest = '%s'\n", dest);

}