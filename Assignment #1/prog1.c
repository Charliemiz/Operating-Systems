#include <pthread.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Global variables
// Holds running total of the natural log
double global_sum = 0.0; 

// Mutex ensures only one thread at a time can modify global_sum
pthread_mutex_t lock;

// Struct to pass multiple pieces of information to threads
typedef struct {
    double x_minus_1;   // x-1 precalculated to decrease repetition
    int thread_id;      // Unique number for each thread
    int num_threads;    // Number of threads being used
    int iterations;     // Number of terms each thread must compute
} thread_data_t;

// Thread function to calculate ln(x)
void* calculate_ln(void* arg) {
    // Extract thread data from argument 
    thread_data_t* data = (thread_data_t*) arg;
    double local_sum = 0.0;

    // Each thread calculates its assigned terms
    for (int i = 0; i < data->iterations; i++) {
        int term_num = data->thread_id + i * data->num_threads + 1;
        double term = pow(data->x_minus_1, term_num) / term_num;

        // Alternating sign for each term
        if (term_num % 2 == 0) {
            term = -term;
        }

        local_sum += term;
    }

    // Protect global_sum update with a mutex lock
    pthread_mutex_lock(&lock);
    global_sum += local_sum;
    pthread_mutex_unlock(&lock);

    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    // Check if 3 arguments are passed
    if (argc != 4) {
        printf("Usage: %s <value (0,2)> <Number of Threads> <Num of Iterations>\n", argv[0]);
        return 1;
    }

    // Parse command-line arguments
    double x = atof(argv[1]);
    int num_threads = atoi(argv[2]);
    int iterations = atoi(argv[3]);     

    // Validate input arguments
    if (x <= 0 || x >= 2 || num_threads < 1 || iterations < 1) {
        printf("Invalid input! Ensure: x is between 0 and 2, number of threads is greater than 0, and iterations are greater than 0.\n");
        return 1;
    }

    // Initialize the mutex lock to control access to global sum
    pthread_mutex_init(&lock, NULL);  // Fix: "pthread_mutext_init" -> "pthread_mutex_init"

    // Declare thread arrays
    pthread_t threads[num_threads];  // Fix: Missing semicolon
    thread_data_t thread_data[num_threads];  // Fix: Missing semicolon

    // Launching Threads...
    // Loop over number of threads and assign data for each thread in thread_data_t struct
    // Creates all threads and executes the calculate_ln function for each thread
    for (int i = 0; i < num_threads; i++) {
        thread_data[i].x_minus_1 = x - 1;
        thread_data[i].thread_id = i;
        thread_data[i].num_threads = num_threads;
        thread_data[i].iterations = iterations;

        pthread_create(&threads[i], NULL, calculate_ln, &thread_data[i]);
    }

    // Joining Threads...
    // Wait for all threads to finish
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // Output the calculated result and the math library's result to confirm
    printf("%.14f\n", global_sum);
    printf("%.14f\n", log(x));

    // Clean up the mutex
    pthread_mutex_destroy(&lock);

    return 0;
}
