#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

static long long nsec_diff(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1000000000LL + (end.tv_nsec - start.tv_nsec);
}

static void create_test_file(const char *path, size_t file_size) {
    int fd = open(path, O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if (fd < 0) {
        fprintf(stderr, "open for write failed: %s\n", strerror(errno));
        exit(1);
    }

    char buf[4096];
    for (size_t i = 0; i < sizeof(buf); i++) {
        buf[i] = (char)('A' + (i % 26));
    }

    size_t written = 0;
    while (written < file_size) {
        size_t to_write = sizeof(buf);
        if (file_size - written < to_write) {
            to_write = file_size - written;
        }

        ssize_t ret = write(fd, buf, to_write);
        if (ret < 0) {
            fprintf(stderr, "write test file failed: %s\n", strerror(errno));
            close(fd);
            exit(1);
        }
        written += (size_t)ret;
    }

    fsync(fd);
    close(fd);
}

static long long run_read_bench(const char *path, size_t buffer_size, long iterations) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "open for read failed: %s\n", strerror(errno));
        exit(1);
    }

    char *buf = malloc(buffer_size);
    if (!buf) {
        fprintf(stderr, "malloc failed\n");
        close(fd);
        exit(1);
    }

    volatile uint64_t checksum = 0;
    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    for (long i = 0; i < iterations; i++) {
        ssize_t ret = read(fd, buf, buffer_size);

        if (ret < 0) {
            fprintf(stderr, "read failed: %s\n", strerror(errno));
            free(buf);
            close(fd);
            exit(1);
        }

        if (ret == 0) {
            if (lseek(fd, 0, SEEK_SET) < 0) {
                fprintf(stderr, "lseek failed: %s\n", strerror(errno));
                free(buf);
                close(fd);
                exit(1);
            }
            i--;
            continue;
        }

        checksum += (uint64_t)buf[0];
    }

    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    if (checksum == 0xdeadbeef) {
        fprintf(stderr, "ignore: %llu\n", (unsigned long long)checksum);
    }

    free(buf);
    close(fd);

    return nsec_diff(start, end);
}

static void print_result(const char *environment, size_t buffer_size, long iterations, long long total_ns) {
    double seconds = (double)total_ns / 1000000000.0;
    double iops = (double)iterations / seconds;
    double total_mb = ((double)buffer_size * (double)iterations) / (1024.0 * 1024.0);
    double mbps = total_mb / seconds;

    printf("%s,baseline,%zu,%ld,%lld,%.6f,%.2f,%.2f\n",
           environment, buffer_size, iterations, total_ns, seconds, iops, mbps);
}

int main(int argc, char **argv) {
    const char *environment = "arta_arm64_vm";
    const char *path = "/dev/shm/ub_read_iops_testfile.dat";
    size_t file_size = 128 * 1024 * 1024;
    long iterations = 2000000;

    if (argc >= 2) {
        iterations = atol(argv[1]);
    }

    create_test_file(path, file_size);

    size_t sizes[] = {64, 256, 1024, 4096};
    size_t n = sizeof(sizes) / sizeof(sizes[0]);

    printf("environment,mode,buffer_size,iterations,total_ns,seconds,iops,mb_per_sec\n");

    for (size_t i = 0; i < n; i++) {
        long long ns = run_read_bench(path, sizes[i], iterations);
        print_result(environment, sizes[i], iterations, ns);
    }

    unlink(path);
    return 0;
}
