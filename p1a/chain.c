#include <stdio.h>


void b(void) {
    int y = 10;
    printf("in b's frame y = %d\n", y);
}

void a(void) {
    b();
    int x = 20;
    printf("in a's frame x = %d\n", x);
}
 
int main(void) {
    int m = 5;
    a();
}