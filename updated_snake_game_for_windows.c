#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>  // Include Windows API header
#include <conio.h>   // Include for _kbhit() and _getch()

#define bgcolor 1
#define ROW 15
#define COL 30
int wait_time_ms = 1000; // This is the maximum value
int border_wall;
char prev = 'd'; // primary direction of movement
const char snake_body = '=', point = 'o', OBSTACLE = '#'; // Visuals

char *OBS[ROW * COL];
char area[ROW][COL];
char *snake[ROW * COL];
int pointr = -1, pointc = -1;
int snakelen = 1;
int num_of_obs = 0;

void obstacle(int obs) {
    if (!obs)
        return;
    printf("Percentage of obstacle(1-50%%): ");
    int per;
    scanf("%d", &per);

    int min = 0, maxR = ROW - 1, maxC = COL - 1, r, c, i = 0;
    for (; i < (per / 100.0) * ROW * COL; i++) {
    generate_obs:
        r = min + rand() % (maxR + 1);
        c = min + rand() % (maxC + 1);
        if (r == 0 && c == 0)
            goto generate_obs;
        if (area[r][c] == OBSTACLE)
            goto generate_obs;
        OBS[i] = &area[r][c];
    }
    num_of_obs = i;
}

void spawn() {
    if (pointr == -1) {
        int min = 0, maxR = ROW - 1, maxC = COL - 1, r, c;
    generate:
        r = min + rand() % (maxR + 1);
        c = min + rand() % (maxC + 1);

        for (int i = 0; i < snakelen; i++) {
            if (snake[i] == &area[r][c])
                goto generate;
        }
        for (int i = 0; i < num_of_obs; i++) {
            if (OBS[i] == &area[r][c])
                goto generate;
        }
        pointr = r;
        pointc = c;
    }
    area[pointr][pointc] = point;
}

char wait(int ms) {
    if (ms == -1) {
        return _getch(); // Use _getch for immediate character input
    }
    float elps = 0;
    while (elps < ms) {
        if (_kbhit())  // Check if a key has been hit
            return _getch();
        else
            Sleep(20);  // Use Sleep instead of usleep
        elps += 20;    // Increase elapsed time by 20 ms
    }
    return 0;
}

char *dir() {
    char ch;
unpaused:
    ch = wait(wait_time_ms);

    if (ch == ' ') {
        while ((ch = wait(-1)) != ' ')
            ;
        goto unpaused;
    }
    if (!ch || !(ch == 'd' || ch == 'a' || ch == 'w' || ch == 's' ||
                 ch == 'D' || ch == 'A' || ch == 'W' || ch == 'S')) {
        ch = prev;
    }

chk:
    switch (ch) {
    case 'D':
    case 'd': // right
        if (snakelen == 1 || (prev != 'a' && prev != 'A')) {
            snake[0]++;
            prev = ch;
            return snake[0] - 1;
        } else {
            ch = prev;
            goto chk;
        }
    case 'A':
    case 'a': // left
        if (snakelen == 1 || (prev != 'd' && prev != 'D')) {
            snake[0]--;
            prev = ch;
            return snake[0] + 1;
        } else {
            ch = prev;
            goto chk;
        }
    case 'W':
    case 'w': // up
        if (snakelen == 1 || (prev != 's' && prev != 'S')) {
            snake[0] -= COL;
            prev = ch;
            return snake[0] + COL;
        } else {
            ch = prev;
            goto chk;
        }
    case 'S':
    case 's': // down
        if (snakelen == 1 || (prev != 'w' && prev != 'W')) {
            snake[0] += COL;
            prev = ch;
            return snake[0] - COL;
        } else {
            ch = prev;
            goto chk;
        }
    }
}

void clear() {
  char buff[(sizeof("\033[A\033[2K") + 5) * (ROW + 5)];
  int size_of_buff = 0;

  for (int i = 1; i <= ROW + 1 + border_wall; i++) {
    size_of_buff +=
        sprintf(buff + size_of_buff, "\033[A"); // move cursor up by 1 row
    size_of_buff += printf(buff + size_of_buff, "\033[2K"); // clear entire row
  }
  printf("%s", buff);
  memset(area, ' ', sizeof(area));
}
int check(char *past, int obs) {
    // Border checking
    if (border_wall) {
        int row = (snake[0] - &area[0][0]) / COL,
            col = (snake[0] - &area[0][0]) % COL;
        int p_col = (past - &area[0][0]) % COL;
        if (p_col == COL - 1 && col == 0)
            return -1;
        else if (p_col == 0 && (col < 0 || col == COL - 1))
            return -1;

        else if (snake[0] < &area[0][0])
            return -1;
        else if (row >= ROW)
            return -1;

    } else {
        int row = (snake[0] - &area[0][0]) / COL,
            col = (snake[0] - &area[0][0]) % COL;
        int p_col = (past - &area[0][0]) % COL;
        if (p_col == COL - 1 && col == 0)
            snake[0] -= COL;
        else if (p_col == 0 && (col < 0 || col == COL - 1))
            snake[0] += COL;
        else {
            if (snake[0] < &area[0][0])
                row = ROW;
            else if (row >= ROW)
                row = 0;

            snake[0] = &area[row][col];
        }
    }
    for (int i = 2; i < snakelen; i++) {
        if (snake[0] == snake[i])
            return -1;
    }
    for (int i = 0; i < num_of_obs; i++) {
        if (OBS[i] == snake[0])
            return -1;
    }
    if (snake[0] == &area[pointr][pointc]) {
        printf("\a");
        return 1;
    }
    return 0;
}

void move(int eaten, char *past) {
    int i = 1;
    // head design
    if (prev == 'd' || prev == 'D')
        (*snake[0]) = '>';
    else if (prev == 'a' || prev == 'A')
        (*snake[0]) = '<';
    else if (prev == 'w' || prev == 'W')
        (*snake[0]) = '^';
    else
        (*snake[0]) = '+';

    for (; i < snakelen; i++) {
        char *temp = snake[i];

        snake[i] = past;
        (*snake[i]) = snake_body;
        past = temp;
    }
    if (snakelen > 1)
        (*snake[i - 1]) = '*'; // tail design
    if (eaten) {
        ++snakelen;
        pointr = -1;
        pointc = -1;
    }
}

void print() {
    int obstacles = 0;

    for (int r = 0; r < ROW; r++) {
        if (border_wall)
            printf("|");
        for (int c = 0; c < COL; c++) {
            if (bgcolor)
                printf("\033[48;5;155m");

            for (int i = 0; i < num_of_obs; i++) {
                if (OBS[i] == &area[r][c]) {
                    printf("\033[38;5;196m%c", OBSTACLE);
                    obstacles = 1;
                    break;
                }
            }

            if (obstacles) {
                obstacles = 0;
                printf("\033[0m");
                continue;
            }
            printf("\033[30m%c", area[r][c]);
            printf("\033[0m");
        }
        if (border_wall)
            printf("|");
        printf("\n");
    }

    printf("\033[0m");
    if (border_wall) {
        for (int i = 0; i < COL + 2; i++)
            printf("|");
        puts("");
    }
    printf("Score: %d\n", snakelen - 1);
}

int main() {
    srand(time(NULL));
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hConsole, &mode);
    SetConsoleMode(hConsole, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    snake[0] = &area[0][0];
    memset(area, ' ', sizeof(area));
    printf("Level(1-100): ");

    unsigned short int level;
    scanf("%hu", &level);

    wait_time_ms /= level;
    printf("Do you want obstacles?(1/0): ");
    short int obs;

    scanf("%hd", &obs);
    obstacle(obs);
    printf("Enable border wall(1/0): ");
    scanf("%d", &border_wall);

    printf("\033[?25l"); // hide cursor
    fflush(stdout);
    printf("Use w/a/s/d for controls. Press <space> to pause/unpause\n");
    Sleep(1000); // Sleep for 1 second
    // No need for set_input_mode since we use _getch

    if (border_wall) {
        border_wall = 1;
        for (int i = 0; i < COL + 2; i++)
            printf("|");
        puts("");
    }

    while (1) {
        print();
        char *past = dir(); // takes time
        clear();
        spawn();
        int c = check(past, obs), eaten = 0;
        if (c == -1)
            break;
        else if (c == 1)
            eaten = 1;
        move(eaten, past);
    }

    printf("\a\a\a");
    Sleep(1000);
    printf("\a\a\a");
    Sleep(1000);
    printf("\a\a\a");

    system("cls"); // Clear the console screen on game over
    printf("\033[1mGAME OVER\033[0m\n");

    Sleep(2000); // Sleep for 2 seconds

    printf("\nScore: %d\n", snakelen - 1);
    Sleep(500); // Sleep for half a second
    printf("\033[?25h"); // show cursor

    return 0;
}
