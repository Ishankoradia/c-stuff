/*
    * main.c
    *
    * My predictions
    * struct S1 { char a; char b; char c; }; = total 3 bytes
    * struct S2 { char c; double d; }; = 16 bytes
    * struct S3 { char a; int b; char c; }; = 12 bytes
    * struct S4 { int b; char a; char c; }; = 8 bytes
*/
#include <stdio.h>

int main() {
    struct S1 {char a; char b; char c;};
    struct S2 {char c; double d;};
    struct S3 {char a; int b; char c;};
    struct S4 {int b; char a; char c;};

    printf("Size of struct S1 %zu\n", sizeof(struct S1));
    printf("Size of struct S2 %zu\n", sizeof(struct S2));
    printf("Size of struct S3 %zu\n", sizeof(struct S3));
    printf("Size of struct S4 %zu\n", sizeof(struct S4));
}

