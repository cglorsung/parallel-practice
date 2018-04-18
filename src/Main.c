#include "MainConfig.h"
#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>

int hello() {
    printf("Hello, world!\n");
    int x = 0, y = 1;
    int *z;

    z = &x;
    printf("VALOF z = &x: %d | Expecting integer memory address\n", z);

    y = *z;
    printf("VALOF y = *z: %d | Expecting 0\n", y);

    x = (int) z;
    printf("VALOF x = z : %d | Expecting integer memory address\n", x);

    *z = 3;
    printf("VALOF *z = 3: x=%d ; y=%d ; z = %d\n", x, y, z);

    //Print expected values for last assignment
    printf("Expecting x, y, z = 3, 0, intMemAddr");

    return 0;
}

int main() {
    hello();
    return 0;
}
