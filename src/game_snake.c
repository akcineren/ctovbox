#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>

#define ROWS 15
#define COLS 15

typedef struct
{
    int x, y;
} Point;

char **grid;
Point snake[ROWS * COLS];
int snake_length = 1;
Point bait;
bool running = true;
char last_direction = 'd';
bool exit_game = false;
int score = 0;

struct termios original_tio; // Global variable for terminal settings

void initialize_grid();
void place_bait();
void move_snake();
void render();
void handle_signal(int signal);
void cleanup();
int kbhit();
void update_grid();
bool check_collision(Point next);
bool is_opposite_direction(char new_direction);
bool play_again_prompt();
void restore_terminal();

int main()
{
    // Save the original terminal settings
    if (tcgetattr(STDIN_FILENO, &original_tio) == -1)
    {
        perror("Failed to get terminal attributes");
        exit(EXIT_FAILURE);
    }

    // Set the terminal to non-canonical mode and disable echo
    struct termios new_tio = original_tio;
    new_tio.c_lflag &= ~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSANOW, &new_tio) == -1)
    {
        perror("Failed to set terminal attributes");
        exit(EXIT_FAILURE);
    }

    // Register restore_terminal with atexit
    if (atexit(restore_terminal) != 0)
    {
        fprintf(stderr, "Failed to register restore_terminal with atexit\n");
        exit(EXIT_FAILURE);
    }

    signal(SIGUSR1, handle_signal);
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
    while (!exit_game)
    {

        initialize_grid();
        snake[0] = (Point){ROWS / 2, COLS / 2};
        grid[snake[0].x][snake[0].y] = 'O';
        place_bait();
        running = true;
        last_direction = 'd';
        score = 0;
        snake_length = 1;
        char input = '\0';
        char last_processed_input = '\0';
        while (running && !exit_game)
        {
            render();

            if (kbhit())
            {
                char current_input = getchar();
                if (current_input != last_processed_input)
                {
                    last_processed_input = current_input;

                    if (current_input == 'q')
                    {
                        raise(SIGUSR1);
                    }
                    else if ((current_input == 'w' || current_input == 'a' || current_input == 's' || current_input == 'd') &&
                             !is_opposite_direction(current_input))
                    {
                        last_direction = current_input;
                    }
                }
                while (kbhit())
                {
                    getchar();
                }
            }

            move_snake();
            usleep(200000);
        }
        cleanup();

        if (exit_game)
        {
            break;
        }
        printf("\nGame Over! Final Score: %d\n", score);
        if (!play_again_prompt())
        {
            break;
        }
    }

    printf("\nExiting the game...\n");
    return 0;
}

void restore_terminal()
{
    // Restore the original terminal settings
    if (tcsetattr(STDIN_FILENO, TCSANOW, &original_tio) == -1)
    {
        perror("Failed to restore terminal attributes");
    }
}

void initialize_grid()
{
    grid = (char **)malloc(ROWS * sizeof(char *));
    for (int i = 0; i < ROWS; i++)
    {
        grid[i] = (char *)malloc(COLS * sizeof(char));
        for (int j = 0; j < COLS; j++)
        {
            grid[i][j] = '.';
        }
    }
}

void place_bait()
{
    srand(time(NULL));
    do
    {
        bait.x = rand() % ROWS;
        bait.y = rand() % COLS;
    } while (grid[bait.x][bait.y] != '.');
    grid[bait.x][bait.y] = 'X';
}

void move_snake()
{
    Point next = snake[0];

    if (last_direction == 'w')
        next.x--;
    else if (last_direction == 'a')
        next.y--;
    else if (last_direction == 's')
        next.x++;
    else if (last_direction == 'd')
        next.y++;

    if (check_collision(next))
    {
        running = false;
        return;
    }

    if (next.x == bait.x && next.y == bait.y)
    {
        score += 10;
        snake[snake_length] = snake[snake_length - 1];
        snake_length++;
        place_bait();
    }
    for (int i = snake_length - 1; i > 0; i--)
    {
        snake[i] = snake[i - 1];
    }
    snake[0] = next;
    update_grid();
}

void render()
{
    printf("\033[H\033[J");
    printf("Score: %d\n\n", score);
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            printf("%c ", grid[i][j]);
        }
        printf("\n");
    }
}
void update_grid()
{
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            if (grid[i][j] != 'X')
            {
                grid[i][j] = '.';
            }
        }
    }
    for (int i = 0; i < snake_length; i++)
    {
        if (i == 0)
        {
            grid[snake[i].x][snake[i].y] = 'O';
        }
        else
        {
            grid[snake[i].x][snake[i].y] = '#';
        }
    }
}
bool check_collision(Point next)
{
    if (next.x < 0 || next.x >= ROWS || next.y < 0 || next.y >= COLS)
    {
        return true;
    }

    for (int i = 0; i < snake_length; i++)
    {
        if (snake[i].x == next.x && snake[i].y == next.y)
        {
            return true;
        }
    }

    return false;
}

void handle_signal(int signal)
{
    if (signal == SIGUSR1 || signal == SIGINT || signal == SIGTERM)
    {
        running = false;
        cleanup();
        restore_terminal(); // Restore terminal settings
        printf("\nGame received signal %d. Returning to main menu...\n", signal);
        exit(2);
    }
}

void cleanup()
{
    for (int i = 0; i < ROWS; i++)
    {
        free(grid[i]);
    }
    free(grid);
    restore_terminal(); // Ensure terminal is restored
}
int kbhit()
{
    int bytesWaiting;
    ioctl(STDIN_FILENO, FIONREAD, &bytesWaiting);
    return bytesWaiting > 0;
}
bool is_opposite_direction(char new_direction)
{
    if (new_direction == last_direction)
    {
        return true;
    }
    return (last_direction == 'w' && new_direction == 's') ||
           (last_direction == 's' && new_direction == 'w') ||
           (last_direction == 'a' && new_direction == 'd') ||
           (last_direction == 'd' && new_direction == 'a');
}

bool play_again_prompt()
{
    printf("\nDo you want to play again? (a: Yes, d: No)\n");
    char choice;
    while (true)
    {
        if (kbhit())
        {
            choice = getchar();
            if (choice == 'a')
            {
                return true;
            }
            else if (choice == 'd')
            {
                return false;
            }
            else if (choice == 'q')
            {
                exit_game = true;
                return false;
            }
        }
    }
}
