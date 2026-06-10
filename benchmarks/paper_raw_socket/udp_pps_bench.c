#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <netinet/in.h>

static volatile sig_atomic_t stop_flag = 0;

static void on_alarm(int sig) {
    (void)sig;
    stop_flag = 1;
}

static long long nsec_diff(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1000000000LL + (end.tv_nsec - start.tv_nsec);
}

static void fill_payload(char *buf, size_t size) {
    for (size_t i = 0; i < size; i++) {
        buf[i] = (char)('A' + (i % 26));
    }
}

static void receiver(int port, int duration_seconds) {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        fprintf(stderr, "receiver socket failed: %s\n", strerror(errno));
        exit(1);
    }

    int rcvbuf = 64 * 1024 * 1024;
    setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &rcvbuf, sizeof(rcvbuf));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons((uint16_t)port);

    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        fprintf(stderr, "bind failed: %s\n", strerror(errno));
        close(fd);
        exit(1);
    }

    signal(SIGALRM, on_alarm);
    alarm(duration_seconds);

    char buf[2048];
    uint64_t packets = 0;
    volatile uint64_t checksum = 0;

    while (!stop_flag) {
        ssize_t ret = recv(fd, buf, sizeof(buf), 0);
        if (ret < 0) {
            if (errno == EINTR) {
                break;
            }
            fprintf(stderr, "recv failed: %s\n", strerror(errno));
            break;
        }

        if (ret > 0) {
            packets++;
            checksum += (uint64_t)buf[0];
        }
    }

    if (checksum == 0xdeadbeef) {
        fprintf(stderr, "ignore: %llu\n", (unsigned long long)checksum);
    }

    printf("%llu\n", (unsigned long long)packets);
    fflush(stdout);
    close(fd);
}

static uint64_t sender(int port, size_t packet_size, int duration_seconds) {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        fprintf(stderr, "sender socket failed: %s\n", strerror(errno));
        exit(1);
    }

    int sndbuf = 64 * 1024 * 1024;
    setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &sndbuf, sizeof(sndbuf));

    struct sockaddr_in dst;
    memset(&dst, 0, sizeof(dst));
    dst.sin_family = AF_INET;
    dst.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    dst.sin_port = htons((uint16_t)port);

    char *buf = malloc(packet_size);
    if (!buf) {
        fprintf(stderr, "malloc failed\n");
        close(fd);
        exit(1);
    }

    fill_payload(buf, packet_size);

    struct timespec start, now;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    uint64_t sent = 0;

    while (1) {
        ssize_t ret = sendto(fd, buf, packet_size, 0, (struct sockaddr *)&dst, sizeof(dst));
        if (ret < 0) {
            if (errno == ENOBUFS || errno == EAGAIN) {
                continue;
            }
            fprintf(stderr, "sendto failed: %s\n", strerror(errno));
            break;
        }

        sent++;

        if ((sent & 0x3fff) == 0) {
            clock_gettime(CLOCK_MONOTONIC_RAW, &now);
            long long ns = nsec_diff(start, now);
            if (ns >= (long long)duration_seconds * 1000000000LL) {
                break;
            }
        }
    }

    free(buf);
    close(fd);
    return sent;
}

static void run_one(size_t packet_size, int duration_seconds, int port) {
    int pipefd[2];
    if (pipe(pipefd) < 0) {
        fprintf(stderr, "pipe failed: %s\n", strerror(errno));
        exit(1);
    }

    pid_t child = fork();
    if (child < 0) {
        fprintf(stderr, "fork failed: %s\n", strerror(errno));
        exit(1);
    }

    if (child == 0) {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        receiver(port, duration_seconds);
        exit(0);
    }



    close(pipefd[1]);
    usleep(200000);

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    uint64_t sent = sender(port, packet_size, duration_seconds);

    waitpid(child, NULL, 0);
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    char output[128] = {0};
    ssize_t n = read(pipefd[0], output, sizeof(output) - 1);
    close(pipefd[0]);

    uint64_t received = 0;
    if (n > 0) {
        received = strtoull(output, NULL, 10);
    }

    double seconds = (double)nsec_diff(start, end) / 1000000000.0;
    double sent_pps = (double)sent / seconds;
    double received_pps = (double)received / seconds;

    printf("arta_arm64_vm,baseline,%zu,%d,%llu,%llu,%.6f,%.2f,%.2f,none\n",
           packet_size,
           duration_seconds,
           (unsigned long long)sent,
           (unsigned long long)received,
           seconds,
           sent_pps,
           received_pps);
}

int main(int argc, char **argv) {
    int duration_seconds = 12;

    if (argc >= 2) {
        duration_seconds = atoi(argv[1]);
    }

    printf("environment,mode,packet_size,duration_seconds,packets_sent,packets_received,seconds,sent_pps,received_pps,computation\n");

    run_one(128, duration_seconds, 19001);
    run_one(512, duration_seconds, 19002);
    run_one(1472, duration_seconds, 19003);

    return 0;
}
