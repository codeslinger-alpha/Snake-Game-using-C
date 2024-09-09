#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#define SIZE 20
char *OBS[SIZE * SIZE];
int wait_time_ms = 100;
char area[SIZE][SIZE];
char prev = 'd';
char *snake[SIZE * SIZE];
int pointr = -1, pointc = -1;
int snakelen = 1;
int num_of_obs = 0;
void obstacle(int obs) {
  if (!obs)
    return;
  printf("Percentage of obstacle(1-50%%): ");
  int per;
  scanf("%d", &per);

  int min = 0, max = SIZE - 1, r, c, i = 0;
  for (; i < (( 0.1*per) / 100.0) * SIZE * SIZE; i++) {
  generate_obs:
    r = min + rand() % (max + 1), c = min + rand() % (max + 1);
    if (r == 0 && c == 0)
      goto generate_obs;
    if (area[r][c] == 'X')
      goto generate_obs;
    OBS[i] = &area[r][c];
  }
  num_of_obs = i;
}

void set_input_mode(int enabled) {
  static struct termios oldt, newt;
  if (enabled) {
    tcgetattr(STDIN_FILENO, &oldt); // Save terminal settings
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);        // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); // Set new terminal settings
  } else {
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // Restore terminal settings
  }
}

// Non-blocking kbhit() replacement
int kbhit() {
  struct timeval tv = {0L, 0L};
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(STDIN_FILENO, &fds);
  return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
}

// getch() replacement
int getch() {
  int ch;
  ch = getchar();
  return ch;
}

void spawn() {
  if (pointr == -1) {
    int min = 0, max = SIZE - 1, r, c;
  generate:
    r = min + rand() % (max + 1), c = min + rand() % (max + 1);

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
  area[pointr][pointc] = '*';
}

char wait(int ms) {
  int elps = 0;
  while (elps < ms) {
    if (kbhit())
      return getch();
    else
      usleep(7000);
    elps += 7;
  }
  return 0;
}

char *dir() {
  char ch = wait(wait_time_ms);
  if (!ch || !(ch == 'd' || ch == 'a' || ch == 'w' || ch == 's'))
    ch = prev;

chk:
  switch (ch) {
  case 'd':
    if (snakelen == 1 || (snake[0] + 1) != snake[1]) {
      snake[0]++;
      prev = ch;
      return snake[0] - 1;
    } else {
      ch = prev;
      goto chk;
    }
  case 'a':
    if (snakelen == 1 || snake[0] - 1 != snake[1]) {
      snake[0]--;
      prev = ch;
      return snake[0] + 1;
    } else {
      ch = prev;
      goto chk;
    }

  case 'w':
    if (snakelen == 1 || snake[0] - SIZE != snake[1]) {
      snake[0] -= SIZE;
      prev = ch;
      return snake[0] + SIZE;
    } else {
      ch = prev;
      goto chk;
    }

  case 's':
    if (snakelen == 1 || snake[0] + SIZE != snake[1]) {
      snake[0] += SIZE;
      prev = ch;
      return snake[0] - SIZE;
    } else {
      ch = prev;
      goto chk;
    }
  }
}

void clear() {

  for (int i = 1; i <= SIZE; i++) {
    printf("\033[A");
    printf("\033[2K");
  }
  // system("clear");
  memset(area, ' ', sizeof(area));
}

int check(char *past, int obs) {
  // Border checking
  int row = (snake[0] - &area[0][0]) / SIZE,
      col = (snake[0] - &area[0][0]) % SIZE;
  int p_col = (past - &area[0][0]) % SIZE;
  if (p_col == SIZE - 1 && col == 0)
    snake[0] -= SIZE;
  else if (p_col == 0 && (col < 0 || col == SIZE - 1))
    snake[0] += SIZE;
  else {
    if (snake[0] < &area[0][0])
      row = SIZE;
    else if (row >= SIZE)
      row = 0;

    snake[0] = &area[row][col];
  }

  for (int i = 2; i < snakelen; i++) {
    if (snake[0] == snake[i])
      return -1;
  }
  for (int i = 0; i < num_of_obs; i++) {
    if (OBS[i] == snake[0])
      return -1;
  }
  if (snake[0] == &area[pointr][pointc])
    return 1;
  return 0;
}

void move(int eaten, char *past) {
  int i = 1;
  (*snake[0]) = '.';
  for (; i < snakelen; i++) {
    char *temp = snake[i];
    snake[i] = past;
    (*snake[i]) = '.';
    past = temp;
  }
  if (eaten) {
    ++snakelen;
    snake[snakelen - 1] = past;
    (*snake[snakelen - 1]) = '.';
    pointr = -1;
    pointc = -1;
  }
}

void print() {
  int o = 0;
  for (int r = 0; r < SIZE; r++) {
    for (int c = 0; c < SIZE; c++) {
      for (int i = 0; i < num_of_obs; i++) {
        if (OBS[i] == &area[r][c]) {
          printf("\033[45m\033[32mX\033[0m");
          o = 1;
          break;
        }
      }
      if (o) {
        o = 0;
        continue;
      }
      printf("\033[45m\033[32m%c\033[0m", area[r][c]);
    }
    printf("\n");
  }
}

int main() {
  snake[0] = &area[0][0];
  memset(area, ' ', sizeof(area));
  printf("Level(1-10): ");

  unsigned short int level;
  scanf("%hu", &level);
  if (level > 10)
    level = 10;
  wait_time_ms *= (11 - level);
  printf("Do you want obstacles?(1/0): ");
  short int obs;
  scanf("%hd", &obs);
  obstacle(obs);
  printf("Use w/a/s/d for controls\n");
  usleep(1000000);
  set_input_mode(1);
  srand(time(NULL));

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

  set_input_mode(0);

  system("clear");
  printf("\033[1mGAME OVER\033[0m\n");

  usleep(2000000);
  printf("\nScore: %d\n", snakelen - 1);

  return 0;
}
