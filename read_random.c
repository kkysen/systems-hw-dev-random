//
// Created by kkyse on 10/27/2017.
//

#include <stdlib.h>
#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

typedef unsigned int uint;

#define PRINT true

#define NUM_INTS 10
#define FILE "random_ints.bin"

#define p(s) printf(s"\n")

#define check_alloc(mem) if (!(mem)) perror("malloc failed"), exit(ENOMEM)

int random_fd = -1;

void *random_bytes(size_t num_bytes) {
    if (random_fd == -1) {
        random_fd = open("/dev/random", O_RDONLY);
    }
    if (random_fd == -1) {
        perror("opening /dev/random");
        return NULL;
    }
    void *const bytes = malloc(num_bytes);
    if (!bytes) {
        perror("malloc failed");
        return NULL;
    }
    p("actually reading");
    const ssize_t num_bytes_read = read(random_fd, bytes, num_bytes);
    if (num_bytes_read == -1) {
        perror("reading /dev/random");
        return NULL;
    }
    p("actually read");
    return bytes;
}

void print_ints(const char *const prefix, const int *const ints, const size_t num_ints) {
//    #if (!PRINT)
//    return;
//    #endif
    for (uint i = 0; i < num_ints; i++) {
        printf("%s%u: %d\n", prefix, i, ints[i]);
    }
}

ssize_t write_ints(const char *const filename, const int *const ints, const size_t num_ints) {
    const int fd = open(filename, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        return -1;
    }
    const ssize_t num_bytes_written = write(fd, ints, num_ints * sizeof(*ints));
    if (num_bytes_written == -1) {
        return -2;
    }
    if (close(fd) == -1) {
        return -3;
    }
    return num_bytes_written;
}

ssize_t read_ints(const char *const filename, int *const ints, const size_t num_ints) {
    const int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        return -1;
    }
    const ssize_t num_bytes_read = read(fd, ints, num_ints * sizeof(*ints));
    if (num_bytes_read == -1) {
        return -2;
    }
    if (close(fd) == -1) {
        return -3;
    }
    return num_bytes_read;
}

void test(size_t num_ints) {
    p("If generating random numbers stalls, then /dev/random might be empty");

    printf("NUM_INTS: %u\n\n", NUM_INTS);
    p("Generating random ints:");
    const int *const random_ints = (int *) random_bytes(num_ints * sizeof(int));
    check_alloc(random_ints);
    print_ints("\trandom ", random_ints, num_ints);
    p();
    
    p("Writing ints to file");
    if (write_ints(FILE, random_ints, num_ints) < 0) {
        perror("write_ints");
        exit(EXIT_FAILURE);
    }
    
    int *const ints_from_file = (int *) malloc(num_ints * sizeof(int));
    check_alloc(ints_from_file);
    
    p("Reading ints from file");
    if (read_ints(FILE, ints_from_file, num_ints) < 0) {
        perror("read_ints");
        exit(EXIT_FAILURE);
    }
    
    p();
    if (memcmp(random_ints, ints_from_file, num_ints * sizeof(int)) != 0) {
        p("Written ints are not the same:");
    } else {
        p("Verification that written ints are the same:");
    }
    print_ints("\trandom ", ints_from_file, num_ints);
    
    free((int *) random_ints);
    free(ints_from_file);
}

int main() {
    test(NUM_INTS);
}