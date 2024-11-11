#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int sum(int *vect, int size)
{
    int total = 0;
    for (int i = 0; i < size; i++)
    {
        total += vect[i];
    }
    return total;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Open the file
    int fd = open(argv[1], O_RDONLY);
    if (fd == -1)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Get the size of the file
    struct stat sb;
    if (fstat(fd, &sb) == -1)
    {
        perror("Error getting file size");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Map the file into memory
    int *data = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (data == MAP_FAILED)
    {
        perror("Error mapping file");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Calculate the number of elements in the file
    int num_elements = sb.st_size / sizeof(int);

    // Call the sum function
    int result = sum(data, num_elements);
    printf("The sum is: %d\n", result);

    // Clean up
    munmap(data, sb.st_size);
    close(fd);

    return EXIT_SUCCESS;
}
