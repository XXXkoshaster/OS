#include "../inc/parent.h"

void get_program_path(char *progpath, size_t size) {
    uint32_t path_size = (uint32_t)size;
    if (_NSGetExecutablePath(progpath, &path_size) != 0) {
        const char msg[] = "error: failed to read full program path\n";
        write(STDERR_FILENO, msg, sizeof(msg));
        exit(EXIT_FAILURE);
    }

    char *last_slash = strrchr(progpath, '/');
    if (last_slash != NULL) {
        *last_slash = '\0';
    }
}

void create_pipe(int channel[2]) {
    if (pipe(channel) == -1) {
        const char msg[] = "error: failed to create pipe\n";
        write(STDERR_FILENO, msg, sizeof(msg));
        exit(EXIT_FAILURE);
    }
}

void handle_child_process(int channel[2], char *progpath, char *filename) {
    pid_t pid = getpid();
    {
        char msg[64];
        const int32_t length = snprintf(msg, sizeof(msg), "%d: I'm a child\n", pid);
        write(STDOUT_FILENO, msg, length);
    }

    int file = open(filename, O_RDONLY);
    if (file == -1) {
        const char msg[] = "error: failed to open requested file\n";
        write(STDERR_FILENO, msg, sizeof(msg));
        exit(EXIT_FAILURE);
    }

    dup2(file, STDIN_FILENO);
    close(file);

    dup2(channel[1], STDOUT_FILENO);
    close(channel[0]);
    close(channel[1]);

    char path[1024];
    snprintf(path, sizeof(path) - 1, "%s/%s", progpath, CLIENT_PROGRAM_NAME);

    char *const args[] = {CLIENT_PROGRAM_NAME, filename, NULL};
    int32_t status = execv(path, args);

    if (status == -1) {
        const char msg[] = "error: failed to exec into new executable image\n";
        write(STDERR_FILENO, msg, sizeof(msg));
        exit(EXIT_FAILURE);
    }
}

void handle_parent_process(int channel[2], pid_t child) {
    pid_t pid = getpid();
    {
        char msg[64];
        const int32_t length = snprintf(msg, sizeof(msg), "%d: I'm a parent, my child has PID %d\n", pid, child);
        write(STDOUT_FILENO, msg, length);
    }

    close(channel[1]);

    char buf[4096];
    ssize_t bytes;
    while ((bytes = read(channel[0], buf, sizeof(buf))) > 0)
        write(STDOUT_FILENO, buf, bytes);

    close(channel[0]);

    int child_status;
    wait(&child_status);

    if (child_status != EXIT_SUCCESS) {
        const char msg[] = "error: child exited with error\n";
        write(STDERR_FILENO, msg, sizeof(msg));
        exit(child_status);
    }
}