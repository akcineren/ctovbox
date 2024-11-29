#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define GRID_HEIGHT 5
#define GRID_WIDTH 20
#define BLOCK_LENGTH 3
#define DROP_SPEED 500000 // Microseconds (0.5 seconds)

typedef struct
{
    int x;  // X-coordinate of the left-most position
    int y;  // Y-coordinate (height)
    int dx; // Direction of horizontal movement (1 = right, -1 = left)
} Block;

char grid[GRID_HEIGHT][GRID_WIDTH];

void initialize_grid()
{
    for (int i = 0; i < GRID_HEIGHT; i++)
    {
        for (int j = 0; j < GRID_WIDTH; j++)
        {
            grid[i][j] = '.';
        }
    }
}

void draw_grid(int score)
{
    clear();
    mvprintw(0, 0, "Score: %d", score); // Display score at the top
    for (int i = 0; i < GRID_HEIGHT; i++)
    {
        for (int j = 0; j < GRID_WIDTH; j++)
        {
            mvaddch(i + 1, j, grid[i][j]); // Offset grid display by 1 for score
        }
    }
    refresh();
}

void place_block(Block block)
{
    for (int i = 0; i < BLOCK_LENGTH; i++)
    {
        if (block.x + i >= 0 && block.x + i < GRID_WIDTH)
        {
            grid[block.y][block.x + i] = '#';
        }
    }
}

void remove_block(Block block)
{
    for (int i = 0; i < BLOCK_LENGTH; i++)
    {
        if (block.x + i >= 0 && block.x + i < GRID_WIDTH)
        {
            grid[block.y][block.x + i] = '.';
        }
    }
}

int can_place_block(Block block)
{
    // Check if the block has reached the bottom
    if (block.y == GRID_HEIGHT - 1)
        return 1;

    for (int i = 0; i < BLOCK_LENGTH; i++)
    {
        if (block.x + i >= 0 && block.x + i < GRID_WIDTH)
        {
            // Check directly below the block
            if (grid[block.y + 1][block.x + i] == '#')
                return 1;
        }
    }

    // Check right-down position when moving right
    if (block.dx == 1)
    {
        if (block.x + BLOCK_LENGTH < GRID_WIDTH &&
            grid[block.y + 1][block.x + BLOCK_LENGTH - 1] == '#')
        {
            return 1;
        }
    }

    // Check left-down position when moving left
    if (block.dx == -1)
    {
        if (block.x > 0 &&
            grid[block.y + 1][block.x] == '#')
        {
            return 1;
        }
    }

    return 0; // Block can move further
}

void finalize_block(Block block)
{
    for (int i = 0; i < BLOCK_LENGTH; i++)
    {
        if (block.x + i >= 0 && block.x + i < GRID_WIDTH)
        {
            grid[block.y][block.x + i] = '#';
        }
    }
}

int is_game_over(Block active_block)
{
    for (int i = 0; i < GRID_WIDTH; i++)
    {
        int is_part_of_active_block = 0;
        for (int j = 0; j < BLOCK_LENGTH; j++)
        {
            if (active_block.y == 0 && i == active_block.x + j)
            {
                is_part_of_active_block = 1;
                break;
            }
        }
        if (!is_part_of_active_block && grid[0][i] == '#')
        {
            return 1;
        }
    }
    return 0;
}

int main()
{
    int play_again = 1;

    while (play_again)
    {
        initscr();
        noecho();
        curs_set(FALSE);
        nodelay(stdscr, TRUE);
        srand(time(NULL));

        initialize_grid();

        Block current_block = {rand() % (GRID_WIDTH - BLOCK_LENGTH), 0, rand() % 2 == 0 ? 1 : -1};
        int running = 1;
        int score = 0;
        time_t last_update = time(NULL);

        while (running)
        {
            remove_block(current_block);

            int ch = getch();
            if (ch == 'q')
            {
                running = 0; // Quit the game
            }
            else if (ch == 's')
            {
                // Drop the block immediately
                while (!can_place_block(current_block))
                {
                    current_block.y++;
                }
                finalize_block(current_block);

                // Initialize the next block
                current_block.x = rand() % (GRID_WIDTH - BLOCK_LENGTH);
                current_block.y = 0;
                current_block.dx = rand() % 2 == 0 ? 1 : -1;
            }

            // Automatically move the block downward
            if (!can_place_block(current_block))
            {
                current_block.y++;
            }
            else
            {
                finalize_block(current_block);

                // Initialize the next block
                current_block.x = rand() % (GRID_WIDTH - BLOCK_LENGTH);
                current_block.y = 0;
                current_block.dx = rand() % 2 == 0 ? 1 : -1;
            }

            // Move the block horizontally
            current_block.x += current_block.dx;
            if (current_block.x < 0 || current_block.x + BLOCK_LENGTH > GRID_WIDTH)
            {
                current_block.dx *= -1; // Reverse direction
                current_block.x += current_block.dx;
            }

            place_block(current_block);

            // Check for game over
            if (is_game_over(current_block))
            {
                running = 0;
            }

            // Update score every second
            if (time(NULL) - last_update >= 1)
            {
                score++;
                last_update = time(NULL);
            }

            draw_grid(score);
            usleep(DROP_SPEED);
        }

        clear();
        mvprintw(GRID_HEIGHT / 2, (GRID_WIDTH - 10) / 2, "Game Over!");
        mvprintw(GRID_HEIGHT / 2 + 1, (GRID_WIDTH - 16) / 2, "Final Score: %d", score);
        mvprintw(GRID_HEIGHT / 2 + 3, (GRID_WIDTH - 36) / 2, "Do you want to play again?");
        mvprintw(GRID_HEIGHT / 2 + 4, (GRID_WIDTH - 28) / 2, "Press 'a' for YES or 'd' for NO");
        refresh();

        // Wait for user input for play again
        int choice;
        do
        {
            choice = getch();
        } while (choice != 'a' && choice != 'd');

        if (choice == 'd')
        {
            play_again = 0;
        }

        clear();  // Clear screen before exiting or restarting
        endwin(); // Properly exit ncurses
    }

    printf("Thank you for playing!\n");
    return 0;
}
