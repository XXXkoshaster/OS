#include "../inc/parent.h"

void get_input(char *input, size_t size);

int main() {
    char progpath[4096];
    get_program_path(progpath, sizeof(progpath));

    char file[4096];

    {
        char msg[32];
        const int32_t length = snprintf(msg, sizeof(msg), "Print file name:\n");
        write(STDOUT_FILENO, msg, length);    
    }

    get_input(file, sizeof(file));

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
        handle_child_process(channel, progpath, file);
        break;

    default:
        handle_parent_process(channel, child);
        break;
    }

    return 0;
}

void get_input(char *input, size_t size) 
{
    if (fgets(input, size, stdin) == NULL) {   
        char msg[32];
        const int32_t length = snprintf(msg, sizeof(msg), "error: failed to read input\n");
        write(STDERR_FILENO, msg, sizeof(msg));
        exit(EXIT_FAILURE);
    }

    size_t len = strlen(input);
    if (len > 0 && input[len - 1] == '\n') {
        input[len - 1] = '\0';
    }
}