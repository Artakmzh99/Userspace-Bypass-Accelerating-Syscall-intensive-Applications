#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <errno.h>

static long long nsec_diff(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1000000000LL + (end.tv_nsec - start.tv_nsec);
}

static long long now_nsec_loop_empty(long iterations) {
    struct timespec start, end;
    volatile long sink = 0;

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    for (long i = 0; i < iterations; i++) {
        sink += i;
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    return nsec_diff(start, end);
}

static long long bench_getpid(long iterations) {
    struct timespec start, end;
    volatile pid_t pid;

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    for (long i = 0; i < iterations; i++) {
        pid = getpid();
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    return nsec_diff(start, end);
}

static long long bench_read_zero(long iterations) {
    int fd = open("/dev/zero", O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "open /dev/zero failed: %s\n", strerror(errno));
        exit(1);
    }

    char buf[1];
    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    for (long i = 0; i < iterations; i++) {
        if (read(fd, buf, 1) != 1) {
            fprintf(stderr, "read failed: %s\n", strerror(errno));
            close(fd);
            exit(1);
        }
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    close(fd);
    return nsec_diff(start, end);
}

static long long bench_write_null(long iterations) {
    int fd = open("/dev/null", O_WRONLY);
    if (fd < 0) {
        fprintf(stderr, "open /dev/null failed: %s\n", strerror(errno));
        exit(1);
    }

    char buf[1] = {'x'};
    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    for (long i = 0; i < iterations; i++) {
        if (write(fd, buf, 1) != 1) {
            fprintf(stderr, "write failed: %s\n", strerror(errno));
            close(fd);
            exit(1);
        }
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    close(fd);
    return nsec_diff(start, end);
}

static void print_result(const char *name, long iterations, long long total_ns) {
    double avg_ns = (double) total_ns / (double) iterations;
    printf("%s,%ld,%lld,%.2f\n", name, iterations, total_ns, avg_ns);
}

int main(int argc, char **argv) {
    long iterations = 1000000;

    if (argc >= 2) {
        iterations = atol(argv[1]);
        if (iterations <= 0) {
            fprintf(stderr, "Usage: %s [iterations]\n", argv[0]);
            return 1;
        }
    }

    printf("benchmark,iterations,total_ns,avg_ns\n");

    long long empty_ns = now_nsec_loop_empty(iterations);
    long long getpid_ns = bench_getpid(iterations);
    long long read_ns = bench_read_zero(iterations);
    long long write_ns = bench_write_null(iterations);

    print_result("empty_loop", iterations, empty_ns);
    print_result("getpid", iterations, getpid_ns);
    print_result("read_1B_dev_zero", iterations, read_ns);
    print_result("write_1B_dev_null", iterations, write_ns);

    return 0;
}
