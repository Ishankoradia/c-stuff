#include <stdio.h>

void print_ele2(int *arr, size_t s) {
    for (size_t i=0; i < s; i++) {
        printf("a[%zu] = %d\n",i,*(arr+i));
    }
}


void print_ele1(int arr[]) {
    for (size_t i=0; i < sizeof(arr)/sizeof(*(arr)); i++) {
        printf("a[%zu] = %d\n",i,*(arr+i));
    }
}

int main(void) {
    int a[4] = {10,20,30,40};

    // print_ele1(a);
    print_ele2(a, sizeof(a)/sizeof(*(a)));
}