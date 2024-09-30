//#include <pthread.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Global variables

// holds running total of the natural log
double global_sum = 0.0; 
// mutex ensures only one thread at a time can modify global_sum
//pthread_mutex_t lock;

// Since we need to pass multiple pieces of info, we can just use a struct
typedef struct {
    double x_minus_1;   // x-1 precalculated to decrease repetition
    int thread_id;      // unique number for each thread
    int num_threads;    // number threads being used
    int iterations;     // number terms each thread must compute
} thread_data_t;

void* calculate_ln(void* arg) {
    // Extract thread data from arugment 
    // Perform calculations for each thread
    // Mutex is used to safely update global_sum
    

}

int main(int argc, char* argv[]) {
    // Check if 3 arguments are passed
    // Parse those 3 arguments if so
    if (argc != 4) {
        printf("Usage: %s <value (0,2)> <Number of Threads> <Num of Iterations>\n", argv[0]);
        return 1;
    }

    // Capture given arguments into variables below, 
    // Made sure they are converted to the correct data type (float, int)
    double x = atof(argv[1]);
    int num_threads = atoi(argv[2]);
    int iterations = atoi(argv[3]);     

    if (x <= 0 || x >= 2 || num_threads < 1 || iterations < 1) {
        printf("Invalid input, ensure: number to calculate is between 0 and 2, number of threads is greater than 0 and iterations / terms is greater than 0 as well!");
        return 1;
    }

    
}