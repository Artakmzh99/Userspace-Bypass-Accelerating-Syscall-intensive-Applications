#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>
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

static void receiver_loop(int fd, size_t total_bytes) {
    char buf[65536];
    size_t received = 0;

    while (received < total_bytes) {
        ssize_t ret = recv(fd, buf, sizeof(buf), 0);
        if (ret < 0) {
            fprintf(stderr, "recv failed: %s\n", strerror(errno));
            exit(1);
        }
        if (ret == 0) {
            break;
        }
        received += (size_t)ret;
    }
}

static long long send_benchmark(size_t total_bytes, size_t chunk_size) {
    int sv[2];

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) < 0) {
        fprintf(stderr, "socketpair failed: %s\n", strerror(errno));
        exit(1);
    }

    fflush(stdout);

    pid_t child = fork();
    if (child < 0) {
        fprintf(stderr, "fork failed: %s\n", strerror(errno));
        exit(1);
    }

    if (child == 0) {
        close(sv[0]);
        receiver_loop(sv[1], total_bytes);
        close(sv[1]);
        _exit(0);
    }

    close(sv[1]);

    char *buf = malloc(chunk_size);
    if (!buf) {
        fprintf(stderr, "malloc failed\n");
        close(sv[0]);
        exit(1);
    }

    fill_buffer(buf, chunk_size);

    size_t sent = 0;
    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    while (sent < total_bytes) {
        size_t to_send = chunk_size;
        if (total_bytes - sent < chunk_size) {
            to_send = total_bytes - sent;
        }

        ssize_t ret = send(sv[0], buf, to_send, 0);
        if (ret < 0) {
            fprintf(stderr, "send failed: %s\n", strerror(errno));
            free(buf);
            close(sv[0]);
            exit(1);
        }

        sent += (size_t)ret;
    }

    shutdown(sv[0], SHUT_WR);
    waitpid(child, NULL, 0);

    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    free(buf);
    close(sv[0]);

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

    printf("total_bytes,chunk_size,send_syscalls,total_ns,seconds,mb_per_sec\n");

    for (size_t i = 0; i < n; i++) {
        long long total_ns = send_benchmark(total_bytes, chunks[i]);
        print_result(total_bytes, chunks[i], total_ns);
    }

    return 0;
}
