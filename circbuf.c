#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>

#define DEFAULT_BUF_SIZE 10240

int main(int argc, char** argv) {
    int c;
    size_t buf_size;
    size_t buf_cursor;
    size_t read_size;
    ssize_t rc;
    char* buf;
    struct sigaction action;

    buf_size = DEFAULT_BUF_SIZE;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = SIG_IGN;
    sigaction(SIGTERM, &action, NULL);

    while (-1 != (c = getopt(argc, argv, "hb:"))) {
        switch (c) {
            case 'h':
                printf("Usage: circbuf -b <buf_size>\n");
                exit(EXIT_SUCCESS);
            case 'b':
                buf_size = (size_t)strtoul(optarg, NULL, 10);
                break;
            default:
                exit(EXIT_FAILURE);
                break;
        }
    }

    buf = malloc(buf_size);
    buf_cursor = 0;

    while (1) {
        read_size = buf_size - buf_cursor;
        rc = read(STDIN_FILENO, buf + buf_cursor, read_size);
        if (rc < 0) {
            perror("read");
            exit(EXIT_FAILURE);
        } else if (rc == 0) {
            break;
        } else {
            buf_cursor += rc;
            if (buf_cursor >= buf_size) {
                buf_cursor -= buf_size;
            }
        }
    }

    if (buf_cursor == 0) {
        rc = write(STDOUT_FILENO, buf, buf_size);
    } else {
        rc = write(STDOUT_FILENO, buf + buf_cursor, buf_size - buf_cursor);
        rc = rc < 0 ? rc : write(STDOUT_FILENO, buf, buf_cursor);
    }

    if (rc < 0) {
        perror("write");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
