#include "main.h"

STATE xorN(const char *filename, int n) {

    FILE* file = fopen(filename, "rb");
    if (!file)
        return ERROR_OPEN_FILE;

    size_t block_size = 1 << (n - 3);

    uint8_t* block = (uint8_t*)calloc(block_size, sizeof(uint8_t));
    if (block == NULL)
        return NULL_PTR;
    
    uint8_t* buffer = (uint8_t*)malloc(block_size);
    if (buffer == NULL)
        return NULL_PTR;

    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, block_size, file)) > 0) {

        if (bytes_read < block_size)
            memset(buffer + bytes_read, 0, block_size - bytes_read);
        
        printf("Read bytes: ");
        for (size_t i = 0; i < bytes_read; i++) 
            printf("%02x ", buffer[i]);
        printf("\n");
            
        for (size_t i = 0; i < block_size; i++)
            block[i] ^= buffer[i];
    }

    printf("result for %s (N=%d): ", filename, n);
    
    for (size_t i = 0; i < block_size; i++)
        printf("%02x", block[i]);

    printf("\n");

    free(block);
    free(buffer);
    fclose(file);

    return DONE;
}

STATE mask_operation(const char *filename, uint32_t mask) {

    FILE* file = fopen(filename, "rb");
    if (!file)
        return ERROR_OPEN_FILE;

    uint32_t value;
    size_t count = 0;

    while (fread(&value, sizeof(uint32_t), 1, file) == 1) {
        printf("Read value: 0x%X\n", value);
        if ((value & mask) == mask)
            count++;
    }

    printf("File %s: %zu numbers matches", filename, count);
    fclose(file);

    return DONE;
}

STATE copyN_operation(const char *filename, int n) {
    pid_t pid = fork();
    
    if (pid < 0) {
        return ERROR_CREATE_PID;

    } else if (pid == 0) {

        FILE *src_file = fopen(filename, "r");
        if (!src_file)
            return ERROR_OPEN_FILE;

        for (int i = 1; i <= n; i++) {
            char copy_filename[NAME_MAX];
            snprintf(copy_filename, sizeof(copy_filename), "%s_%d", filename, i);
            
            FILE *copy_file = fopen(copy_filename, "w");
            if (!copy_file) {
                fclose(src_file);
                return ERROR_OPEN_FILE;
            }

            rewind(src_file);

            char c;
            while ((c = fgetc(src_file)) != EOF)
                fputc(c, copy_file);

            fclose(copy_file);
        }

        fclose(src_file);
    }

    return DONE;
}

STATE find_operation(const char* filename, const char* string) {
    pid_t pid = fork();
    
    if (pid < 0) {
        fprintf(stderr, "Failed to create process\n");
        return ERROR_CREATE_PID;
    } else if (pid == 0) {
        FILE *file = fopen(filename, "r");
        if (!file) {
            fprintf(stderr, "Error opening file: %s\n", filename);
            exit(EXIT_FAILURE);
        }

        fseek(file, 0, SEEK_END);
        long file_size = ftell(file);
        if (file_size == -1) {
            fprintf(stderr, "Error getting file size: %s\n", filename);
            fclose(file);
            exit(EXIT_FAILURE);
        }
        fseek(file, 0, SEEK_SET);

        char *buffer = malloc(file_size + 1);
        if (buffer == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            fclose(file);
            exit(EXIT_FAILURE);
        }
            
        size_t bytes_read = fread(buffer, 1, file_size, file);
        if (bytes_read != (size_t)file_size) {
            fprintf(stderr, "Error reading file: %s\n", filename);
            free(buffer);
            fclose(file);
            exit(EXIT_FAILURE);
        }
        buffer[bytes_read] = '\0';

        int found = (strstr(buffer, string) != NULL);
            
        if (found) 
            printf("String found in: %s\n", filename);

        free(buffer);
        if (fclose(file) != 0) {
            fprintf(stderr, "Error closing file: %s\n", filename);
            exit(EXIT_FAILURE);
        }
        exit(found ? EXIT_SUCCESS : EXIT_FAILURE);
    }

    return DONE;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s filez operation (params)\n", argv[0]);
        return UNAVALABLE_INPUT;
    }

    int num_files = argc - 2;
    char* operation = argv[argc - 1];
    STATE status; 

    if (strncmp(operation, "xor", 3) == 0) {
        
        int n = atoi(operation + 3);
        if (n < 2 || n > 6) {
            printf("Invalid input\n");
            return UNAVALABLE_INPUT;
        }
        
        for (int i = 0; i < num_files; i++) {
            status = xorN(argv[i + 1], n);
            handler(status);
        }

    } else if (strncmp(operation, "mask", 4) == 0) {
        
        char *mask_str = argv[argc - 1];
        uint32_t mask = strtoul(mask_str + 4, NULL, 16);
        
        for (int i = 0; i < num_files; i++) {
            status = mask_operation(argv[i + 1], mask);
            handler(status);
        }

    } else if (strncmp(operation, "copy", 4) == 0) {

        int n = atoi(operation + 4);
        
        if (n <= 0) printf("Invalid input\n");
        
        for (int i = 0; i < num_files; i++) {
            status = copyN_operation(argv[i + 1], n);
            handler(status);
        }

        for (int i = 0; i < num_files; i++)
            wait(NULL);

    } else if (strncmp(operation, "find", 4) == 0) {

        char *search_string = argv[argc - 1] + 4;
        int any_found = 0;
        
        printf("%s\n", search_string);

        for (int i = 0; i < num_files; i++) {
            status = find_operation(argv[i + 1], search_string);
            handler(status);
        }

        for (int i = 0; i < num_files; i++) {
            int status;
            wait(&status);
            if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS)
                any_found = 1;
        }
        
        if (!any_found) 
            printf("String not found in any file\n");

    } else {
        fprintf(stderr, "Invalid operation or missing parameters\n");
    }

    return 0;
}
