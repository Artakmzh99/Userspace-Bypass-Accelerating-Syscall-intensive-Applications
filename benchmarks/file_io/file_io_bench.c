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

static void fill_buffer(char *buf, size_t size) {
    for (size_t i = 0; i < size; i++) {
        buf[i] = (char)('A' + (i % 26));
    }
}

static long long write_file(const char *path, size_t total_bytes, size_t chunk_size) {
    int fd = open(path, O_CREAT | O_TRUNC | O_WRONLY, 0644);
    if (fd < 0) {
        fprintf(stderr, "open failed: %s\n", strerror(errno));
        exit(1);
    }

    char *buf = malloc(chunk_size);
    if (!buf) {
        fprintf(stderr, "malloc failed\n");
        close(fd);
        exit(1);
    }

    fill_buffer(buf, chunk_size);

    size_t written = 0;
    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    while (written < total_bytes) {
        size_t to_write = chunk_size;
        if (total_bytes - written < chunk_size) {
            to_write = total_bytes - written;
        }

        ssize_t ret = write(fd, buf, to_write);
        if (ret < 0) {
            fprintf(stderr, "write failed: %s\n", strerror(errno));
            free(buf);
            close(fd);
            exit(1);
        }

        written += (size_t)ret;
    }

    fsync(fd);

    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    free(buf);
    close(fd);

    return nsec_diff(start, end);
}

static void print_result(size_t total_bytes, size_t chunk_size, long long total_ns) {
    double seconds = (double)total_ns / 1000000000.0;
    double mb = (double)total_bytes / (1024.0 * 1024.0);
    double mbps = mb / seconds;
    long syscall_count = (long)((total_bytes + chunk_size - 1) / chunk_size);

    printf("%zu,%zu,%ld,%lld,%.6f,%.2f\n",
           total_bytes, chunk_size, syscall_count, total_ns, seconds, mbps);
}

int main(int argc, char **argv) {
    size_t total_bytes = 64 * 1024 * 1024;

    if (argc >= 2) {
        total_bytes = (size_t)atoll(argv[1]);
    }

    const char *path = "/tmp/file_io_bench_output.dat";

    size_t chunks[] = {
        1,
        4,
        16,
        64,
        256,
        1024,
        4096,
        16384,
        65536
    };

    size_t n = sizeof(chunks) / sizeof(chunks[0]);

    printf("total_bytes,chunk_size,write_syscalls,total_ns,seconds,mb_per_sec\n");

    for (size_t i = 0; i < n; i++) {
        long long total_ns = write_file(path, total_bytes, chunks[i]);
        print_result(total_bytes, chunks[i], total_ns);
        unlink(path);
    }

    return 0;
}
