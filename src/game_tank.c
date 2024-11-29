#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <time.h> // Add this header for the time function

#define ROWS 15
#define COLS 20
#define EMPTY '.'
#define TANK 'T'
#define BULLET '|'
#define ENEMY 'E'

// Game elements
char grid[ROWS][COLS];
int tank_x = ROWS - 2;
int tank_y = COLS / 2;
int score = 0;
int running = 1;

void handle_exit(int signal)
{
    if (signal == SIGINT || signal == SIGTERM)
    {
        running = 0; // Stop the game loop
        printf("\nGame received signal %d. Exiting gracefully...\n", signal);
        exit(2); // Exit with a specific code for signal termination
    }
}

// Initialize the game grid
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

// Display the game grid
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

// Spawn an enemy at a random position
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
                // Clear the current bullet position
                grid[i][j] = EMPTY;

                // Check if the bullet is still in bounds
                if (i > 0)
                {
                    if (grid[i - 1][j] == ENEMY)
                    {
                        // Bullet hits an enemy
                        grid[i - 1][j] = EMPTY;
                        score++;
                    }
                    else if (grid[i - 1][j] == EMPTY)
                    {
                        // Move bullet to the next position
                        grid[i - 1][j] = BULLET;
                    }
                }
            }
        }
    }
}

// Move enemies downward
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
// Handle tank movement
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

    // Check if the target cell contains an enemy
    if (grid[new_x][new_y] == ENEMY)
    {
        printf("\nGame Over! Final Score: %d\n", score);
        running = 0;
        return; // Exit the function to prevent further updates
    }

    // Update the tank's position
    grid[tank_x][tank_y] = EMPTY;
    tank_x = new_x;
    tank_y = new_y;
    grid[tank_x][tank_y] = TANK;
}

// Shoot a bullet
void shoot_bullet()
{
    if (tank_x > 0 && grid[tank_x - 1][tank_y] == EMPTY)
    {
        grid[tank_x - 1][tank_y] = BULLET;
    }
}

// Read user input without pressing enter
char get_input()
{
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
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
            // Ignore invalid inputs
            continue;
        }

        usleep(200000); // Game speed
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