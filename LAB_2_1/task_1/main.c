#include "main.h"

void clear_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int check_user(FILE* file, const char* login, int pin, USER* user) {
    int registered = 0;
    char data[128];

    rewind(file); 

    while (fgets(data, sizeof(data), file)) {
        data[strcspn(data, "\n")] = '\0'; 
        
        char curr_login[7];
        int curr_pin, curr_limit = -1;
        
        if (sscanf(data, "Login: %6s Password: %d Limit: %d", curr_login, &curr_pin, &curr_limit) >= 2) {
            if (strcmp(login, curr_login) == 0 && pin == curr_pin) {
                registered = 1;
                strcpy(user->login, curr_login);
                user->pin = curr_pin;
                user->requests_limit = curr_limit != -1 ? curr_limit : -1;
                user->requests_count = 0;
                break;
            }
        }
    }

    return registered;
}

void update_user_limits(const char* username, int limit) {
    FILE* data_file = fopen("./database/data.txt", "r");
    FILE* temp_file = fopen("./database/temp.txt", "w");
    
    if (data_file == NULL || temp_file == NULL) {
        printf("Error opening files\n");
        if (data_file) fclose(data_file);
        if (temp_file) fclose(temp_file);
        return;
    }
    
    char data[128];
    int found = 0;
    
    while (fgets(data, sizeof(data), data_file)) {
        data[strcspn(data, "\n")] = '\0';
        
        char curr_login[7];
        int curr_pin, curr_limit = -1;
        
        if (sscanf(data, "Login: %6s Password: %d Limit: %d", curr_login, &curr_pin, &curr_limit) >= 2) {
            if (strcmp(username, curr_login) == 0) {
                fprintf(temp_file, "Login: %s Password: %d Limit: %d\n", curr_login, curr_pin, limit);
                found = 1;
            } else {
                if (curr_limit != -1) {
                    fprintf(temp_file, "Login: %s Password: %d Limit: %d\n", curr_login, curr_pin, curr_limit);
                } else {
                    fprintf(temp_file, "Login: %s Password: %d\n", curr_login, curr_pin);
                }
            }
        } else {
            fprintf(temp_file, "%s\n", data);
        }
    }
    
    fclose(data_file);
    fclose(temp_file);
    
    if (found) {
        remove("./database/data.txt");
        rename("./database/temp.txt", "./database/data.txt");
        printf("set_limit successfully applied to user %s\n", username);
    } else {
        remove("./database/temp.txt");
        printf("USER %s not found\n", username);
    }
}

STATE register_user() {
    char login[7];
    int pin;

    printf("Choose your login (up to 6 symbols):\n");
    scanf("%6s", login);
    clear_buffer(); 
    
    for (int i = 0; login[i] != '\0'; i++) {
        if (!isalnum(login[i])) {
            printf("Login should contain only Latin letters and digits.\n");
            return DONE;
        }
    }
    
    do {
        printf("Choose your pin (0 <= pin < 100000):\n");
        scanf("%d", &pin); 
        clear_buffer(); 

        if (pin < 0 || pin >= 100000) printf("Incorrect pin. Try again.\n");

    } while (pin < 0 || pin >= 100000);

    FILE* data_file = fopen("./database/data.txt", "a+");
    if (data_file == NULL) {
        return ERROR_OPEN_FILE;
    }

    USER dummy_user;
    if (check_user(data_file, login, pin, &dummy_user)) {
        printf("USER has already been registered.\n");
    } else {
        fprintf(data_file, "Login: %s Password: %d\n", login, pin);
        printf("Registration is successful.\n");
    }

    fclose(data_file);
    return DONE;
}

STATE login_user(USER* user) {
    char login[7];
    int pin;

    printf("Enter your login:\n");
    scanf("%6s", login);
    clear_buffer();
    
    printf("Enter your pin:\n");
    scanf("%d", &pin);
    clear_buffer();

    FILE* data_file = fopen("./database/data.txt", "r");
    if (data_file == NULL) {
        return ERROR_OPEN_FILE;
    }

    if (check_user(data_file, login, pin, user)) {
        printf("Login successful. Welcome, %s!\n", login);
        fclose(data_file);
        return DONE;
    } else {
        printf("Invalid login or pin.\n");
        fclose(data_file);
        return UNAVALABLE_INPUT;
    }
}

void get_time() {
    time_t mytime = time(NULL);
    struct tm* now = localtime(&mytime);
    printf("Time: %02d:%02d:%02d\n", now->tm_hour, now->tm_min, now->tm_sec);
}

void get_date() {
    time_t mytime = time(NULL);
    struct tm* now = localtime(&mytime);
    printf("Date: %02d:%02d:%04d\n", now->tm_mday, now->tm_mon + 1, now->tm_year + 1900);
}

STATE how_much(char* flag) {
    if (flag == NULL || flag[0] != '-' || flag[2] != '\0') {
        printf("Invalid flag format. Use -s, -m, -h, or -y.\n");
        return UNAVALABLE_INPUT;
    }
    
    time_t now;
    time(&now);

    char* option = flag + 1;

    char input_data[128];
    printf("Print date in format d:m:y\n");
    scanf("%s", input_data);
    clear_buffer();
    
    struct tm* start_time = (struct tm*)malloc(sizeof(struct tm));
    if (start_time == NULL) {
        printf("Error allocation memory\n");
        return NULL_PTR;
    }
    
    start_time->tm_sec = 0;
    start_time->tm_min = 0;
    start_time->tm_hour = 0;
    start_time->tm_isdst = -1;
    
    int curr_c = 0;
    int counter = 0;
    while (input_data[curr_c] != '\0') { 
        while (!isdigit(input_data[curr_c]) && input_data[curr_c] != '\0') curr_c++;
        if (isdigit(input_data[curr_c]) && input_data[curr_c] != '\0'){
            int num = 0;
            while (isdigit(input_data[curr_c]) && input_data[curr_c] != '\0') {
                num = num * 10 + (input_data[curr_c] - '0');
                curr_c++;
            }
            counter++;

            if (counter == 1) {
                start_time->tm_mday = num;
            } else if (counter == 2) {
                start_time->tm_mon = num - 1;
            } else if (counter == 3) {
                start_time->tm_year = num - 1900; 
            } else {
                free(start_time);
                return UNAVALABLE_INPUT;
            }
        }
    }

    time_t start = mktime(start_time);
    if (start == -1) {
        printf("Invalid date\n");
        free(start_time);
        return UNAVALABLE_INPUT;
    }

    switch (*option)
    {
    case 's':
        printf("Seconds: %d\n", difftime(now, start));
        break;
    case 'm':
        printf("Minutes: %d\n", (difftime(now, start) / 60));
        break;
    case 'h':
        printf("Hours: %d\n", (difftime(now, start) / 3600));
        break;
    case 'y':
        printf("Years: %d\n", (difftime(now, start)) / (3600.0 * 24 * 365.25));
        break;
    default:
        printf("Invalid flag. Use -s, -m, -h, or -y\n");
        break;
    }

    free(start_time);
    return DONE;
}

STATE set_limit(USER* current_user, char* username) {
    if (username == NULL) {
        printf("USERname is required for set_limit\n");
        return UNAVALABLE_INPUT;
    }
    
    int limit;
    int confirmation;
    
    printf("Enter request limit for user %s:\n", username);
    scanf("%d", &limit);
    clear_buffer();
    
    printf("Enter 12345 to confirm sanction:\n");
    scanf("%d", &confirmation);
    clear_buffer();
    
    if (confirmation != 12345) {
        printf("Sanction not confirmed\n");
        return DONE;
    }
    
    update_user_limits(username, limit);
    return DONE;
}

STATE process_input(USER* user, char* command) {
    char cmd[20];
    char arg[20] = "";
    
    sscanf(command, "%19s %19s", cmd, arg);
    
    if (strcmp(cmd, "Time") == 0) {
        get_time();
    } else if (strcmp(cmd, "Date") == 0) {
        get_date();
    } else if (strcmp(cmd, "Howmuch") == 0) {
        how_much(arg);
    } else if (strcmp(cmd, "Logout") == 0) {
        printf("Logging out...\n");
        return LOGOUT;
    } else if (strcmp(cmd, "set_limit") == 0) {
        set_limit(user, arg);
    } else {
        printf("Unknown command: %s\n", cmd);
    }
    
    return DONE;
}

void input_loop(USER* user) {
    char command[LINE_MAX];
    STATE state = DONE;
    
    printf("Enter commands (Time, Date, Howmuch <flag>, set_limit <username>, Logout):\n");
    
    while (state != LOGOUT) {
        printf("%s> ", user->login);
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = '\0';
        
        user->requests_count++;
        state = process_input(user, command);
        
        if (user->requests_limit > 0 && user->requests_count >= user->requests_limit) {
            printf("You've reached your request limit. Logging out...\n");
            break;
        }
    }
}

int main() {
    int choice;
    STATE result;
    USER current_user;
    
    while (1) {
        printf("\n=== Command Shell ===\n");
        printf("1. Login\n");
        printf("2. Register\n");
        printf("3. Exit\n");
        printf("Choose an option: ");
        
        scanf("%d", &choice);
        clear_buffer();
        
        switch (choice) {
            case 1:
                result = login_user(&current_user);
                if (result == DONE) {
                    input_loop(&current_user);
                } else {
                    handler(result);
                }
                break;
            case 2:
                register_user();
                break;
            case 3:
                printf("Exiting\n");
                return 0;
            default:
                printf("Invalid choice. Try again.\n");
                break;
        }
    }
    
    return 0;
}