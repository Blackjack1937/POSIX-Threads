#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

// Structure to hold thread-specific data for partial sums
typedef struct
{
    int *start; // Pointer to the start of the segment in the vector
    int size;   // Number of elements in the segment
} ThreadData;

// Function for each thread to calculate the partial sum
void *partial_sum(void *arg)
{
    ThreadData *data = (ThreadData *)arg;
    int *segment = data->start;
    int size = data->size;
    int sum = 0;

    for (int i = 0; i < size; i++)
    {
        sum += segment[i];
    }

    // Return the sum as a void* by casting it
    return (void *)(long)sum;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s filename nb_threads\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Read arguments
    char *filename = argv[1];
    int nb_threads = atoi(argv[2]);

    // Open the file
    int fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    // Get the size of the file
    struct stat st;
    if (fstat(fd, &st) == -1)
    {
        perror("Error getting file size");
        close(fd);
        return EXIT_FAILURE;
    }
    int vector_size = st.st_size / sizeof(int); // Number of integers in the file

    // Memory map the file
    int *vect = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (vect == MAP_FAILED)
    {
        perror("Error mapping file");
        close(fd);
        return EXIT_FAILURE;
    }
    close(fd); // File descriptor no longer needed after mmap

    // Determine the number of elements each thread will process
    int elements_per_thread = vector_size / nb_threads;

    pthread_t threads[nb_threads];
    ThreadData thread_data[nb_threads];
    int total_sum = 0;

    // Create threads to calculate partial sums
    for (int i = 0; i < nb_threads; i++)
    {
        thread_data[i].start = &vect[i * elements_per_thread];
        thread_data[i].size = elements_per_thread;
        pthread_create(&threads[i], NULL, partial_sum, &thread_data[i]);
    }

    // Collect the partial results from each thread
    for (int i = 0; i < nb_threads; i++)
    {
        void *partial_result;
        pthread_join(threads[i], &partial_result);
        total_sum += (int)(long)partial_result;
    }

    // Print the final sum
    printf("The total sum is: %d\n", total_sum);

    // Unmap the memory and exit
    if (munmap(vect, st.st_size) == -1)
    {
        perror("Error unmapping memory");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
