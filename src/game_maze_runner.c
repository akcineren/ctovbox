#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>

#define ROWS 15
#define COLS 15

// Function to enable raw mode for instant keypress
void enable_raw_mode()
{
    struct termios term;
    tcgetattr(0, &term);              // Get current terminal attributes
    term.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echo
    tcsetattr(0, TCSANOW, &term);     // Apply changes
}

// Function to disable raw mode
void disable_raw_mode()
{
    struct termios term;
    tcgetattr(0, &term);             // Get current terminal attributes
    term.c_lflag |= (ICANON | ECHO); // Re-enable canonical mode and echo
    tcsetattr(0, TCSANOW, &term);    // Apply changes
}

// Function to initialize the maze
void initialize_maze(char maze[ROWS][COLS], int *player_x, int *player_y)
{
    srand(time(0));
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            maze[i][j] = (rand() % 4 == 0) ? '#' : '.';
        }
    }

    // Place the player at the starting position
    *player_x = 1;
    *player_y = 1;
    maze[*player_x][*player_y] = 'P';

    // Place the exit point
    maze[ROWS - 2][COLS - 2] = 'E';
}

// Function to display the maze
void display_maze(char maze[ROWS][COLS])
{
    system("clear");
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            printf("%c ", maze[i][j]);
        }
        printf("\n");
    }
}

// Function to move the player
int move_player(char maze[ROWS][COLS], int *player_x, int *player_y, char move)
{
    int new_x = *player_x;
    int new_y = *player_y;

    if (move == 'w')
        new_x--;
    else if (move == 's')
        new_x++;
    else if (move == 'a')
        new_y--;
    else if (move == 'd')
        new_y++;

    // Check boundaries and walls
    if (new_x >= 0 && new_x < ROWS && new_y >= 0 && new_y < COLS && maze[new_x][new_y] != '#')
    {
        maze[*player_x][*player_y] = '.'; // Reset old position
        *player_x = new_x;
        *player_y = new_y;

        if (maze[*player_x][*player_y] == 'E')
            return 1; // Player reached the exit

        maze[*player_x][*player_y] = 'P'; // Update to new position
    }

    return 0;
}

// Function to handle the play again prompt
int play_again()
{
    char input;
    printf("\nDo you want to play again? (a: Yes, d: No): ");
    while (1)
    {
        input = getchar();
        if (input == 'q')
        {
            printf("\nExiting the game gracefully. Goodbye!\n");
            exit(0); // Global exit on 'q'
        }
        if (input == 'a')
            return 1;
        if (input == 'd')
            return 0;
    }
}

int main()
{
    char maze[ROWS][COLS];
    int player_x, player_y;
    char input;
    int win = 0;
    time_t start_time, end_time;

    // Enable raw mode for instant key capture
    enable_raw_mode();

    do
    {
        initialize_maze(maze, &player_x, &player_y);
        win = 0;

        printf("\nControls: Use 'w', 'a', 's', 'd' to move. Press 'q' to quit.\n");

        // Record the start time
        time(&start_time);

        while (!win)
        {
            display_maze(maze);
            printf("\nEnter your move: ");

            input = getchar(); // Capture input instantly

            if (input == 'q')
            {
                printf("\nExiting the game gracefully. Goodbye!\n");
                disable_raw_mode(); // Restore terminal settings before exiting
                exit(0);
            }

            win = move_player(maze, &player_x, &player_y, input);
            if (win)
            {
                // Record the end time
                time(&end_time);

                display_maze(maze);
                printf("\nCongratulations! You've reached the exit!\n");
                printf("Time taken: %.2f seconds\n", difftime(end_time, start_time));
            }
        }
    } while (play_again());

    // Disable raw mode and restore terminal settings
    disable_raw_mode();

    printf("\nThanks for playing! Goodbye!\n");
    return 0;
}