#include <stdio.h>
#include <string.h>

void my_strcpy(char *dest, const char *src) {
    while (*src != '\0') {
        *dest = *src;
        src = src + 1;
        dest = dest + 1;  
    }
    *dest = '\0';
}

int main(void) {
    const char *src = "hi there";

    char dest[strlen(src) + 1];

    my_strcpy(dest, src);

    printf("addr of src = %p\n", (void *)src);
    printf("value of src = %s\n", src);
    printf("len of src = %zu\n\n", strlen(src));

    printf("addr of dest = %p\n", (void *)dest);
    printf("value of dest = %s\n", dest);
    printf("len of dest = %zu\n", strlen(dest));
}