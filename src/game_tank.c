#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <time.h>

#define ROWS 15
#define COLS 20
#define EMPTY '.'
#define TANK 'T'
#define BULLET '|'
#define ENEMY 'E'

char grid[ROWS][COLS];
int tank_x = ROWS - 2;
int tank_y = COLS / 2;
int score = 0;
int running = 1;

struct termios original_termios;

void restore_terminal_settings()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
    printf("\nTerminal settings restored. Exiting...\n");
}

void handle_exit(int signal)
{
    running = 0;
    printf("\nGame received signal %d. Exiting gracefully...\n", signal);
    restore_terminal_settings();
    exit(2);
}

void initialize_grid()
{
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            grid[i][j] = EMPTY;
        }
    }
    grid[tank_x][tank_y] = TANK;
}

void display_grid()
{
    system("clear");
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            printf("%c ", grid[i][j]);
        }
        printf("\n");
    }
    printf("\nScore: %d\n", score);
    printf("Controls: w/a/s/d to move, space to shoot, q to quit.\n");
}

void spawn_enemy()
{
    int x = rand() % (ROWS / 2);
    int y = rand() % COLS;
    if (grid[x][y] == EMPTY)
    {
        grid[x][y] = ENEMY;
    }
}

void move_bullets()
{
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            if (grid[i][j] == BULLET)
            {
                grid[i][j] = EMPTY;
                if (i > 0)
                {
                    if (grid[i - 1][j] == ENEMY)
                    {
                        grid[i - 1][j] = EMPTY;
                        score++;
                    }
                    else if (grid[i - 1][j] == EMPTY)
                    {
                        grid[i - 1][j] = BULLET;
                    }
                }
            }
        }
    }
}

void move_enemies()
{
    for (int i = ROWS - 1; i >= 0; i--)
    {
        for (int j = 0; j < COLS; j++)
        {
            if (grid[i][j] == ENEMY)
            {
                grid[i][j] = EMPTY;
                if (i + 1 < ROWS)
                {
                    if (grid[i + 1][j] == TANK)
                    {
                        printf("\nGame Over! Final Score: %d\n", score);
                        running = 0;
                    }
                    else if (grid[i + 1][j] == BULLET)
                    {
                        grid[i + 1][j] = EMPTY;
                        score++;
                    }
                    else
                    {
                        grid[i + 1][j] = ENEMY;
                    }
                }
            }
        }
    }
}

void move_tank(char direction)
{
    int new_x = tank_x;
    int new_y = tank_y;

    if (direction == 'w' && tank_x > 0)
        new_x--;
    if (direction == 's' && tank_x < ROWS - 1)
        new_x++;
    if (direction == 'a' && tank_y > 0)
        new_y--;
    if (direction == 'd' && tank_y < COLS - 1)
        new_y++;

    if (grid[new_x][new_y] == ENEMY)
    {
        printf("\nGame Over! Final Score: %d\n", score);
        running = 0;
        return;
    }
    grid[tank_x][tank_y] = EMPTY;
    tank_x = new_x;
    tank_y = new_y;
    grid[tank_x][tank_y] = TANK;
}

void shoot_bullet()
{
    if (tank_x > 0 && grid[tank_x - 1][tank_y] == EMPTY)
    {
        grid[tank_x - 1][tank_y] = BULLET;
    }
}

char get_input()
{
    struct termios newt;
    char ch;

    tcgetattr(STDIN_FILENO, &original_termios); // Save original settings
    atexit(restore_terminal_settings);          // Register cleanup function

    newt = original_termios;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    ch = getchar();

    return ch;
}

void game_loop()
{
    while (running)
    {
        display_grid();
        if (rand() % 10 < 3)
            spawn_enemy();
        move_enemies();
        move_bullets();

        char input = get_input();
        if (input == 'q')
        {
            handle_exit(SIGINT);
        }
        else if (input == 'w' || input == 'a' || input == 's' || input == 'd')
        {
            move_tank(input);
        }
        else if (input == ' ')
        {
            shoot_bullet();
        }
        else
        {
            continue;
        }

        usleep(200000);
    }
}

int main()
{
    signal(SIGUSR1, handle_exit);
    signal(SIGINT, handle_exit);
    signal(SIGTERM, handle_exit);
    srand(time(NULL));
    initialize_grid();
    game_loop();
    return 0;
}
