#include <chrono>
#include <iostream>
#include <cstdlib>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <NUMPAGES> <reps>" << std::endl;
        return 1; // me forgets to type arguments
    }

    long PAGESIZE = sysconf(_SC_PAGESIZE);
    long jump = PAGESIZE / sizeof(int);
    int NUMPAGES = atoi(argv[1]);
    int reps = atoi(argv[2]);

    if (PAGESIZE % sizeof(int) != 0) {
        std::cerr << "PAGESIZE is not a multiple of sizeof(int)." << std::endl;
        return 1;
    }

    // why you has bug? make me sad.
    std::cout << "PAGESIZE: " << PAGESIZE << std::endl;
    std::cout << "jump (ints per page): " << jump << std::endl;
    std::cout << "Total integers to allocate: " << NUMPAGES * jump << std::endl;

    
    int *a = (int*)calloc(NUMPAGES * jump, sizeof(int));
    if (a == NULL) {
        perror("calloc failed");
        return 1; // Exit if memory allocation fails
    }

    auto start = std::chrono::high_resolution_clock::now();
    
    for (int k = 0; k < reps; k += 1) {
        for (int i = 0; i < NUMPAGES * jump; i += jump) {
            a[i] += 1; // Increment each integer in the array
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Operation took " << duration.count() << " milliseconds." << std::endl;

    // Free the allocated memory
    free(a);
    return 0;
}
/*
1.Using the chrono api you can get time down to the nanosecond. Therefore an operation needs to 
take more than a nanosecond to be measurable give or take some latency.

5.  Compile it with the -o0 flag

6. taskset -c <core_number> ./tlb.c. whenever I run it without this flag it runs faster, i'm
 assuming that it gets to run more frequently when it jumps around to different cpus even if it
  doesn't take advantage of cache affinity it still runs faster on multiple cpus.
7. I mitigated this by initializing the array before I started measuring time using chrono.*/
