#include "../inc/parent.h"

int main(int argc, char **argv) {
    if (argc == 1) {
        char msg[1024];
        uint32_t len = snprintf(msg, sizeof(msg) - 1, "usage: %s filename\n", argv[0]);
        write(STDERR_FILENO, msg, len);
        exit(EXIT_SUCCESS);
    }

    char progpath[1024];
    get_program_path(progpath, sizeof(progpath));

    int channel[2];
    create_pipe(channel);

    const pid_t child = fork();

    switch (child) {
    case -1:
        {
            const char msg[] = "error: failed to spawn new process\n";
            write(STDERR_FILENO, msg, sizeof(msg));
            exit(EXIT_FAILURE);
        }
        break;

    case 0:
        handle_child_process(channel, progpath, argv[1]);
        break;

    default:
        handle_parent_process(channel, child);
        break;
    }

    return 0;
}