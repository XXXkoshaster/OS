#include "main.h"

STATE show_dir_list(const char* dir_path) {
    if (dir_path == NULL) return DIR_PATH_NULL_PTR;

    DIR* input_dir = opendir(dir_path);
    if (input_dir == NULL) return DIR_NOT_FOUND;

    struct dirent* curr_file;
    while ((curr_file = readdir(input_dir)) != NULL) {
        char path[PATH_MAX];
        if (snprintf(path, sizeof(path), "%s/%s", dir_path, curr_file->d_name) < 0) {
            closedir(input_dir);
            return READING_ERROR;
        }

        struct stat file_stat;
        
        if (stat(path, &file_stat)) { 
            closedir(input_dir);
            return STAT_ERROR;
        }

        int inode_number = file_stat.st_ino;

        if (S_ISBLK(file_stat.st_mode))
            printf("%s %s %d\n", curr_file->d_name, "Block pecial file", inode_number);
        else if(S_ISCHR(file_stat.st_mode))
            printf("%s %s %d\n", curr_file->d_name, "Character special file", inode_number);
        else if(S_ISDIR(file_stat.st_mode))
            printf("%s %s %d\n", curr_file->d_name, "Directory", inode_number);
        else if(S_ISFIFO(file_stat.st_mode))
            printf("%s %s %d\n", curr_file->d_name, "Pipe or FIFO special file", inode_number);
        else if(S_ISREG(file_stat.st_mode))
            printf("%s %s %d\n", curr_file->d_name, "Regular file", inode_number);
        else if(S_ISLNK(file_stat.st_mode))
            printf("%s %s %d\n", curr_file->d_name, "Symbolic link", inode_number);
        else
            printf("%s %s %d\n", curr_file->d_name, "Unknown", inode_number);
    }

    closedir(input_dir);
    return DONE;
} 

STATE my_ls(char** dirs_list, int count_dirs) {
    if (dirs_list == NULL || !count_dirs) return UNAVALABLE_INPUT;
    STATE state; 
    for (int i = 0; i < count_dirs; i++) {
        STATE state = show_dir_list(dirs_list[i]);
        if (state != DONE) return state;
    }

    return DONE;
}

int main(int argc, char** argv) {
    printf("Name Type Inode_number\n");
    STATE state = my_ls(argv + 1, argc - 1);
    handler(state);

    return 0;
}