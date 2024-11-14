#include "../inc/child.h"

int main() {
    char buf[256];
    int num;
    ssize_t bytes_read;

    while ((bytes_read = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
        char *ptr = buf;
        while (ptr < buf + bytes_read) {
            char *endptr;
            num = strtol(ptr, &endptr, 10);

            if (ptr == endptr) {
                while (ptr < buf + bytes_read && *ptr != '\n') {
                    ptr++;
                }
                ptr++;
                continue;
            }

            if (num < 0 || is_prime(num)) {
                exit(EXIT_SUCCESS);
            }

            if (!is_prime(num)) {
                char output[32];
                int len = snprintf(output, sizeof(output), "%d\n", num);
                write(STDOUT_FILENO, output, len);
            }

            while (ptr < buf + bytes_read && *ptr != '\n') {
                ptr++;
            }
            ptr++;
        }
    }

    return 0;
}

int is_prime(int num) {
    if (num <= 1)
        return 0;

    if (num <= 3)
        return 1;

    if (num % 2 == 0 || num % 3 == 0)
        return 0;
		
    for (int i = 5; i * i <= num; i += 6)
        if (num % i == 0 || num % (i + 2) == 0)
            return 0;

    return 1;
}