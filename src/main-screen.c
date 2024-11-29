#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <termios.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h> // For struct stat, stat(), and macros like S_ISREG

#define MAX_GAMES 100
#define MAX_CHILDREN 100

char *games[MAX_GAMES];
int game_count = 0;
int current_game = 0;
bool exit_flag = false;
struct termios original_tio;

pid_t child_processes[MAX_CHILDREN];
int child_count = 0;

// Function prototypes
void scan_games(const char *path);
void display_main_screen();
void handle_signal(int sig);
void handle_child_exit(int sig);
void configure_terminal();
void restore_terminal();
void terminate_all_children();
void register_child(pid_t pid);
void log_event(const char *message, pid_t pid);

int main()
{
    struct sigaction sa;

    // Set up signal handler for SIGINT and SIGTERM
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    // Set up signal handler for SIGCHLD to detect game termination
    sa.sa_handler = handle_child_exit;
    sigaction(SIGCHLD, &sa, NULL);

    // Configure terminal for non-canonical mode
    configure_terminal();

    // Log the program's own PID
    log_event("program started", getpid());

    // Scan for games in the current directory
    scan_games("bin");
    if (game_count == 0)
    {
        printf("No games found in the directory.\n");
        restore_terminal();
        return 1;
    }

    while (!exit_flag)
    {
        display_main_screen();

        char input = getchar();

        if (input == 'w')
        {
            current_game = (current_game - 1 + game_count) % game_count;
        }
        else if (input == 's')
        {
            current_game = (current_game + 1) % game_count;
        }
        else if (input == '\n') // Start the selected game
        {
            printf("Launching %s...\n", games[current_game]);

            // Ignore SIGINT while child is running
            signal(SIGINT, SIG_IGN);

            pid_t pid = fork();
            if (pid == 0)
            {
                // Child process: Restore default SIGINT behavior
                signal(SIGINT, SIG_DFL);

                // Launch the selected game
                char game_path[256];
                snprintf(game_path, sizeof(game_path), "bin/%s", games[current_game]);
                execlp(game_path, games[current_game], NULL);
                perror("Failed to launch the selected game");
                exit(1);
            }
            else if (pid > 0)
            {
                // Parent process: Register the child and wait for it to terminate
                register_child(pid);

                // Do not block the main loop while waiting for child termination
                while (waitpid(pid, NULL, WNOHANG) == 0)
                {
                    // Allow handling of signals and refreshing the main menu
                    usleep(100000);
                }

                // Restore signal handler
                signal(SIGINT, handle_signal);

                // Reconfigure terminal settings after child exits
                configure_terminal();
            }
            else
            {
                perror("Failed to fork process");
            }
        }
        else if (input == 'q') // Quit the main menu
        {
            exit_flag = true;
        }
        if (exit_flag)
        {
            break;
        }
    }

    printf("Exiting main menu. Goodbye!\n");

    // Free allocated memory
    for (int i = 0; i < game_count; i++)
    {
        free(games[i]);
    }

    terminate_all_children(); // Ensure all children are terminated
    restore_terminal();       // Restore terminal settings
    return 0;
}

void handle_child_exit(int sig)
{
    int status;
    pid_t pid;

    // Loop to reap all zombie processes
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        printf("\nGame (PID %d) ended. Returning to main menu...\n", pid);

        // Mark the terminated child process
        for (int i = 0; i < child_count; i++)
        {
            if (child_processes[i] == pid)
            {
                child_processes[i] = 0;         // Mark this process as terminated
                log_event("child killed", pid); // Log child termination
                break;
            }
        }

        // Log game exit details
        if (WIFEXITED(status))
        {
            printf("Game exited gracefully with status %d.\n", WEXITSTATUS(status));
        }
        else if (WIFSIGNALED(status))
        {
            printf("Game terminated by signal %d.\n", WTERMSIG(status));
        }
    }
}

// Scan for game executables in the given directory
void scan_games(const char *path)
{
    DIR *d = opendir(path);
    struct dirent *dir;

    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            if (strncmp(dir->d_name, "game_", 5) == 0)
            {
                char full_path[1024];
                snprintf(full_path, sizeof(full_path), "%s/%s", path, dir->d_name);

                struct stat file_stat;
                if (stat(full_path, &file_stat) == 0 && S_ISREG(file_stat.st_mode) && (file_stat.st_mode & S_IXUSR))
                {
                    games[game_count] = strdup(dir->d_name);
                    game_count++;
                    if (game_count >= MAX_GAMES)
                    {
                        printf("Maximum game limit reached.\n");
                        break;
                    }
                }
            }
        }
        closedir(d);
    }
    else
    {
        perror("Error opening directory");
        exit(1);
    }
}

void display_main_screen()
{
    // Move cursor to the top-left corner
    printf("\033[H\033[J");

    printf("=== Video Game Console ===\n");
    printf("Use 'w' and 's' to navigate, 'Enter' to start, 'q' to quit.\n");
    printf("\n");

    for (int i = 0; i < game_count; i++)
    {
        if (i == current_game)
        {
            printf(">> %s <<\n", games[i]);
        }
        else
        {
            printf("   %s\n", games[i]);
        }
    }
}

// Handle SIGINT and SIGTERM for graceful exit
void handle_signal(int sig)
{
    printf("\nReceived signal %d. Exiting gracefully...\n", sig);
    exit_flag = true;
    terminate_all_children(); // Terminate all child processes
    restore_terminal();
    exit(0); // Ensure immediate exit
}

// Configure terminal for non-canonical input
void configure_terminal()
{
    struct termios new_tio;
    tcgetattr(STDIN_FILENO, &original_tio); // Get current terminal settings
    new_tio = original_tio;

    // Disable canonical mode and echoing
    new_tio.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
}

// Restore original terminal settings
void restore_terminal()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &original_tio);
}

void terminate_all_children()
{
    for (int i = 0; i < child_count; i++)
    {
        if (child_processes[i] > 0)
        {
            printf("Terminating child process (PID %d)...\n", child_processes[i]);
            kill(child_processes[i], SIGTERM);
            log_event("child killed", child_processes[i]); // Log child termination
        }
    }
}

void register_child(pid_t pid)
{
    if (child_count < MAX_CHILDREN)
    {
        child_processes[child_count++] = pid;
        log_event("child forked", pid); // Log child creation
    }
    else
    {
        fprintf(stderr, "Too many child processes to track!\n");
    }
}

void log_event(const char *message, pid_t pid)
{
    FILE *log_file = fopen("process_log.txt", "a");
    if (log_file != NULL)
    {
        fprintf(log_file, "%s %d\n", message, pid);
        fclose(log_file);
    }
    else
    {
        perror("Failed to open log file");
    }
}
