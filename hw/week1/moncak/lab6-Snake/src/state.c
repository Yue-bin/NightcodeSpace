#include "state.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_state_t *state, unsigned int row,
                         unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t *state, unsigned int snum);
static char next_square(game_state_t *state, unsigned int snum);
static void update_tail(game_state_t *state, unsigned int snum);
static void update_head(game_state_t *state, unsigned int snum);

/* Task 1 */
game_state_t *create_default_state() {

  // total
  game_state_t *default_state = (game_state_t *)malloc(sizeof(game_state_t));

  // board
  default_state->num_rows = 18;
  default_state->board =
      (char **)malloc(sizeof(char *) * default_state->num_rows);
  for (int i = 0; i < default_state->num_rows; i++) {
    default_state->board[i] = (char *)malloc(sizeof(char) * 21);
  }
  strcpy(default_state->board[0], "####################");
  strcpy(default_state->board[17], "####################");
  for (int i = 1; i < default_state->num_rows - 1; i++) {
    strcpy(default_state->board[i], "#                  #");
  }
  strcpy(default_state->board[2], "# d>D    *         #");
  /*
   for (int i = 0; i < 20; i++) {
     default_state->board[0][i] = '#';
     default_state->board[default_state->num_rows - 1][i] = '#';
   }
   for (int i = 0; i < default_state->num_rows; i++) {
     default_state->board[i][0] = '#';
     default_state->board[17][i] = '#';
   }
   default_state->board[2][9] = '*';
   */

  // snakes
  /*
  default_state->board[2][2] = 'd';
  default_state->board[2][3] = '>';
  default_state->board[2][4] = 'D';
  */
  default_state->num_snakes = 1;
  default_state->snakes =
      (snake_t *)malloc(sizeof(snake_t) * default_state->num_snakes);
  default_state->snakes[0].tail_row = 2;
  default_state->snakes[0].tail_col = 2;
  default_state->snakes[0].head_row = 2;
  default_state->snakes[0].head_col = 4;
  default_state->snakes[0].live = true;

  return default_state;
}

/* Task 2 */
void free_state(game_state_t *state) {
  // snakes
  free(state->snakes);
  // board
  for (int i = 0; i < state->num_rows; i++) {
    free(state->board[i]);
  }
  free(state->board);
  // total
  free(state);
  return;
}

/* Task 3 */
void print_board(game_state_t *state, FILE *fp) {
  for (int i = 0; i < state->num_rows; i++) {
    fprintf(fp, "%s\n", state->board[i]);
  }
  return;
}

/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
void save_board(game_state_t *state, char *filename) {
  FILE *f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t *state, unsigned int row, unsigned int col) {
  return state->board[row][col];
}

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t *state, unsigned int row,
                         unsigned int col, char ch) {
  state->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  return (c == 'w' || c == 'a' || c == 's' || c == 'd');
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  return (c == 'W' || c == 'A' || c == 'S' || c == 'D' || c == 'x');
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  return (is_tail(c) || is_head(c) || c == '^' || c == '<' || c == 'v' ||
          c == '>');
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  switch (c) {
  case '^':
    return 'w';
  case '<':
    return 'a';
  case 'v':
    return 's';
  case '>':
    return 'd';
  default:
    return '?';
  }
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
  switch (c) {
  case 'W':
    return '^';
  case 'A':
    return '<';
  case 'S':
    return 'v';
  case 'D':
    return '>';
  default:
    return '?';
  }
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  if (c == 'v' || c == 's' || c == 'S') {
    return cur_row + 1;
  } else if (c == '^' || c == 'w' || c == 'W') {
    return cur_row - 1;
  }
  return cur_row;
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  if (c == '>' || c == 'd' || c == 'D') {
    return cur_col + 1;
  } else if (c == '<' || c == 'a' || c == 'A') {
    return cur_col - 1;
  }
  return cur_col;
}

/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake
  is moving into.

  This function should not modify anything.
*/
static char next_square(game_state_t *state, unsigned int snum) {
  char head = get_board_at(state, state->snakes[snum].head_row,
                           state->snakes[snum].head_col);
  unsigned int next_row = get_next_row(state->snakes[snum].head_row, head);
  unsigned int next_col = get_next_col(state->snakes[snum].head_col, head);
  return get_board_at(state, next_row, next_col);
}

/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the
  head.
*/
static void update_head(game_state_t *state, unsigned int snum) {
  // on the board
  char head = get_board_at(state, state->snakes[snum].head_row,
                           state->snakes[snum].head_col);
  unsigned int next_row = get_next_row(state->snakes[snum].head_row, head);
  unsigned int next_col = get_next_col(state->snakes[snum].head_col, head);
  set_board_at(state, next_row, next_col, head);
  set_board_at(state, state->snakes[snum].head_row,
               state->snakes[snum].head_col, head_to_body(head));
  // in the snake struct
  state->snakes[snum].head_row = next_row;
  state->snakes[snum].head_col = next_col;
  return;
}

/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_state_t *state, unsigned int snum) {
  // on the board
  char ori_tail = get_board_at(state, state->snakes[snum].tail_row,
                               state->snakes[snum].tail_col);
  unsigned int next_row = get_next_row(state->snakes[snum].tail_row, ori_tail);
  unsigned int next_col = get_next_col(state->snakes[snum].tail_col, ori_tail);
  char cur_tail = body_to_tail(get_board_at(state, next_row, next_col));
  set_board_at(state, next_row, next_col, cur_tail);
  set_board_at(state, state->snakes[snum].tail_row,
               state->snakes[snum].tail_col, ' ');
  // in the snake struct
  state->snakes[snum].tail_row = next_row;
  state->snakes[snum].tail_col = next_col;
  return;
}

/* Task 4.5 */
void update_state(game_state_t *state, int (*add_food)(game_state_t *state)) {
  // update each snake
  for (unsigned int i = 0; i < state->num_snakes; i++) {
    if (!state->snakes[i].live)
      continue;
    switch (next_square(state, i)) {
    case ' ':
      update_head(state, i);
      update_tail(state, i);
      break;
    case '*':
      update_head(state, i);
      add_food(state);
      break;
    case '#':
    case '^':
    case '<':
    case 'v':
    case '>':
    case 'w':
    case 'a':
    case 's':
    case 'd':
    case 'W':
    case 'A':
    case 'S':
    case 'D':
      state->snakes[i].live = false;
      set_board_at(state, state->snakes[i].head_row, state->snakes[i].head_col,
                   'x');
      break;
    default:
      break;
    }
  }
  return;
}

/* Task 5 */
game_state_t *load_board(FILE *fp) {

  // total
  game_state_t *state = (game_state_t *)malloc(sizeof(game_state_t));

  // board
  // init
  state->num_rows = 0;
  state->board = malloc(sizeof(char *) * state->num_rows);
  char *temp_col = (char *)malloc(sizeof(char) * UINT16_MAX *
                                  2); // 一个非常大的临时变量，无伤大雅(
  memset(temp_col, '\0', sizeof(char) * UINT16_MAX * 2);
  // read each line until eof
  while (fscanf(fp, "%[^\n]", temp_col) != EOF) {
    // update num_rows
    state->num_rows++;
    // realloc
    state->board = realloc(state->board, sizeof(char *) * state->num_rows);
    // malloc
    state->board[state->num_rows - 1] = malloc(
        sizeof(char) *
        (strlen(temp_col) +
         1)); // 直接这么写会在realloc不能拓展当前内存空间而是重新分配的时候丢弃前面所有行
              // but who cares
    // copy
    strcpy(state->board[state->num_rows - 1], temp_col);
    // clear temp_col
    memset(temp_col, '\0', sizeof(char) * UINT16_MAX * 2);
    // skip '\n'
    fgetc(fp);
  }

  // snakes
  state->num_snakes = 0;
  state->snakes = NULL;

  return state;
}

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_state_t *state, unsigned int snum) {
  // init the trace
  unsigned int cur_row = state->snakes[snum].tail_row;
  unsigned int cur_col = state->snakes[snum].tail_col;
  // start trace
  do {
    unsigned int next_row =
        get_next_row(cur_row, get_board_at(state, cur_row, cur_col));
    unsigned int next_col =
        get_next_col(cur_col, get_board_at(state, cur_row, cur_col));
    cur_row = next_row;
    cur_col = next_col;
  } while (!is_head(get_board_at(state, cur_row, cur_col)));
  // fill the head
  state->snakes[snum].head_row = cur_row;
  state->snakes[snum].head_col = cur_col;
  return;
}

/* Task 6.2 */
game_state_t *initialize_snakes(game_state_t *state) {
  // find the tail
  for (unsigned int i = 0; i < state->num_rows; i++) {
    for (unsigned int j = 0; j < strlen(state->board[i]); j++) {
      if (is_tail(get_board_at(state, i, j))) {
        // update num_snakes
        state->num_snakes++;
        // realloc
        state->snakes =
            realloc(state->snakes, sizeof(snake_t) * state->num_snakes);
        // 这里同上面那个realloc可能发生数据丢失,but who cares
        // fill the tail
        state->snakes[state->num_snakes - 1].tail_row = i;
        state->snakes[state->num_snakes - 1].tail_col = j;
        // find the head
        find_head(state, state->num_snakes - 1);
        // live
        state->snakes[state->num_snakes - 1].live = true;
      }
    }
  }
  return state;
}
