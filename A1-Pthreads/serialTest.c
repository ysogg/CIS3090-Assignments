#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>


int main(int argc, char* argv[]) {
    clock_t start = clock();
    long long n = strtoll((argv[1]), NULL, 10);
    long double factor = 1.0;
    long double sum = 0.0;
    long double pi;

    
    for (long long j = 0; j < n; j++, factor = -factor ) {
        sum += factor / (2 * j+1);
    }
    pi = 4.0 * sum;
    clock_t end = clock();

    printf("%lld,%.15LF,%0.3f\n", n, pi, (float)(end - start) / CLOCKS_PER_SEC);
    // printf("Computed Pi with %lld iterations (%.15LF) in: %0.3fs\n", n, pi, (float)(end - start) / CLOCKS_PER_SEC);

    return 0;
}
