#include <stdlib.h>
#include <stdio.h>
int main(int argc, const char *argv[]) {
    const char *test = "0.14C";
    double db = strtod(test, NULL);
    printf("%f", db);
}
