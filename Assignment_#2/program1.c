/**
 * Charlie Misbach
 * Operating Systems
 * Programming Assignment #2
 * Due: 10.28.2024 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <semaphore.h>

#define MAX_SEQUENCE_SIZE 1048576  // Maximum size for the sequence (1 MB)
#define MAX_SUBSEQUENCE_SIZE 10240 // Maximum size for the subsequence (10 KB)
#define SHM_NAME "/shared_data" // Name for shared memory
#define SEM_NAME "/semaphore"   // Name for semaphore

/**
 * Structure to hold the best match position and count.
 */
typedef struct {
    int best_match_pos;
    int best_match_count;
} SharedData;

// function declarations
void read_file(const char *filename, char *buffer, int max_size);
void search_subsequence(char *sequence, char *subsequence, int seq_len, int sub_len, int num_processes, int process_id, SharedData *shared_data, sem_t *sem);
SharedData *setup_shared_memory();
sem_t *setup_semaphore();

/**
 * setup_shared_memory - Creates and initializes shared memory.
 * 
 * This function sets up shared memory for storing the best match
 * position and best match count, initializes its values, and 
 * returns a pointer to the shared memory region.
 * 
 * Returns:
 *   SharedData* - Pointer to the shared memory region.
 */
SharedData *setup_shared_memory() {
    // Create shared memory
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open failed");
        exit(EXIT_FAILURE);
    }

    // Set the size of shared memory
    if (ftruncate(fd, sizeof(SharedData)) == -1) {
        perror("ftruncate failed");
        exit(EXIT_FAILURE);
    }

    // Map the shared memory
    SharedData *shared_data = (SharedData *)mmap(NULL, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_data == MAP_FAILED) {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }

    // Initialize shared memory
    shared_data->best_match_pos = -1;
    shared_data->best_match_count = 0;

    return shared_data;
}

/**
 * setup_semaphore - Creates and initializes a semaphore.
 * 
 * This function creates or opens a semaphore for process synchronization.
 * The semaphore is used to control access to shared memory, ensuring 
 * that only one process updates shared memory at a time.
 * 
 * Returns:
 *   sem_t* - Pointer to the created semaphore.
 */
sem_t *setup_semaphore() {
    // Create or open semaphore
    sem_t *sem = sem_open(SEM_NAME, O_CREAT, 0666, 1); // Initial value is 1
    if (sem == SEM_FAILED) {
        perror("sem_open failed");
        exit(EXIT_FAILURE);
    }
    return sem;
}

/**
 * read_file - Reads the contents of a file into a buffer.
 * 
 * This function reads up to max_size bytes from a given file
 * and stores the result in the provided buffer.
 * 
 * Parameters:
 *   filename - The name of the file to read.
 *   buffer - The buffer where file contents will be stored.
 *   max_size - The maximum size to read from the file.
 * 
 * Returns:
 *   void
 */
void read_file(const char *filename, char *buffer, int max_size) {
    FILE *file = fopen(filename, "r");

    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Read file contents into the buffer
    size_t bytes_read = fread(buffer, 1, max_size, file);
    buffer[bytes_read] = '\0'; // null terminate the buffer

    fclose(file);
}

/**
 * search_subsequence - Searches for the best subsequence match.
 * 
 * This function performs a brute-force search for the subsequence
 * within the given sequence. It updates shared memory with the best
 * match position and count if the current match is better than the
 * previous best match.
 * 
 * Parameters:
 *   sequence - The main sequence to search within.
 *   subsequence - The subsequence to search for.
 *   seq_len - The length of the main sequence.
 *   sub_len - The length of the subsequence.
 *   num_processes - The number of processes.
 *   process_id - The ID of the current process.
 *   shared_data - Pointer to shared memory structure.
 *   sem - Pointer to the semaphore for synchronization.
 * 
 * Returns:
 *   void
 */
void search_subsequence(char *sequence, char *subsequence, int seq_len, int sub_len, int num_processes, int process_id, SharedData *shared_data, sem_t *sem) {
    for (int i = process_id; i <= seq_len - sub_len; i += num_processes) {
        int match_count = 0;
        for (int j = 0; j < sub_len; j++) {
            if (sequence[i + j] == subsequence[j]) {
                match_count++;
            }
        }

        // Lock the semaphore before updating shared memory
        sem_wait(sem);
        if (match_count > shared_data->best_match_count) {
            shared_data->best_match_pos = i;
            shared_data->best_match_count = match_count;
        }
        // Unlock the semaphore
        sem_post(sem);
    }
}

/**
 * main - The main function of the program.
 * 
 * This function initializes shared memory and semaphores, reads the
 * input sequence and subsequence files, creates child processes for
 * parallel searching, and prints the final results.
 * 
 * Parameters:
 *   argc - The number of command-line arguments.
 *   argv - The command-line arguments.
 * 
 * Returns:
 *   int - 0 on successful execution.
 */
int main(int argc, char *argv[]) {
    char *sequence_file;
    char *subsequence_file;
    int num_processes;

    if (argc != 4) {
        fprintf(stderr, "Usage ./program1 <Sequence File Name> <Subsequence File Name> <Num Processes>");
        exit(EXIT_FAILURE);
    }

    // Parse the arguments
    sequence_file = argv[1];
    subsequence_file = argv[2];
    num_processes = atoi(argv[3]);

    // Validate number of processes
    if (num_processes <= 0 || num_processes > 20) {
        fprintf(stderr, "Error: Number of processes must be positive.\n");
        exit(EXIT_FAILURE);
    }

    // Print parsed arguments for verification
    printf("Sequence File: %s\n", sequence_file);
    printf("Subsequence File: %s\n\n", subsequence_file);

    // Allocate buffers for sequence and subsequence
    char sequence[MAX_SEQUENCE_SIZE];
    char subsequence[MAX_SUBSEQUENCE_SIZE];

    // Read the files
    read_file(sequence_file, sequence, MAX_SEQUENCE_SIZE);
    read_file(subsequence_file, subsequence, MAX_SUBSEQUENCE_SIZE);

    int seq_len = strlen(sequence);
    int subseq_len = strlen(subsequence);

    // Set up shared memory and semaphores
    SharedData *shared_data = setup_shared_memory();
    sem_t *sem = setup_semaphore();

    // Fork processes
    for (int i = 0; i < num_processes; i++){
        if (fork() == 0) { // child process
            search_subsequence(sequence, subsequence, seq_len, subseq_len, num_processes, i, shared_data, sem);
            exit(0);
        }
    }

    // Wait for all child processes to finish
    for (int i = 0; i < num_processes; i++) {
        wait(NULL);
    }

    // Print results
    printf("Number of Processes: %d\n", num_processes);
    printf("Best Match Position: %d\n", shared_data->best_match_pos);
    printf("Best Match Count: %d\n", shared_data->best_match_count);

    // Clean up shared memory and semaphore
    munmap(shared_data, sizeof(SharedData));
    shm_unlink(SHM_NAME);
    sem_close(sem);
    sem_unlink(SEM_NAME);

    return 0;
}