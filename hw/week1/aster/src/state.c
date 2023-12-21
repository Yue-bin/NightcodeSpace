#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t* state, unsigned int snum);
static char next_square(game_state_t* state, unsigned int snum);
static void update_tail(game_state_t* state, unsigned int snum);
static void update_head(game_state_t* state, unsigned int snum);

/* Task 1 */
game_state_t* create_default_state() {
    game_state_t* new_game_state = (game_state_t*)malloc(sizeof(game_state_t));
    char board[18][21] = {
        "####################",
        "#                  #",
        "# d>D    *         #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "####################" };
    new_game_state->board = (char**)malloc(18 * sizeof(char*));
    for (int i=0; i<18;i++){
      new_game_state->board[i] = (char *)malloc(21 * sizeof(char));
    }
    for (int i=0;i<18;i++){
      for (int j=0;j<21;j++){
        new_game_state->board[i][j] = board[i][j];
      }
    }
    new_game_state->num_rows = 18;
    new_game_state->num_snakes = 1;
    new_game_state->snakes = (snake_t*)malloc(sizeof(snake_t));
    new_game_state->snakes->head_col = 4;
    new_game_state->snakes->head_row = 2;
    new_game_state->snakes->tail_col = 2;
    new_game_state->snakes->tail_row = 2;
    new_game_state->snakes->live = true;
    return new_game_state;
}
/* Task 2 */
void free_state(game_state_t* state) {
  for(int i=0;i<state->num_rows;i++){
    free(state->board[i]);
  }
  free(state->board);
  free(state->snakes);
  free(state);
  return;
}

/* Task 3 */
void print_board(game_state_t* state, FILE* fp) {
  for(int i=0;i<state->num_rows;i++){
    fprintf(fp,"%s\n", state->board[i]);
  }
  return;
}

/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
void save_board(game_state_t* state, char* filename) {
  FILE* f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t* state, unsigned int row, unsigned int col) {
  return state->board[row][col];
}

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  if (c == 'w' || c == 'a' || c == 's' || c == 'd')
  {
    return true;
  }
  else
  {
    return false;
  }
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  if (c == 'W' || c == 'A' || c == 'S' || c == 'D' || c == 'x')
  {
    return true;
  }
  else
  {
    return false;
  }
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
    if (c == 'w' || c == 'a' || c == 's' || c == 'd' 
    || c == 'W' || c == 'A' || c == 'S' || c == 'D' || c == 'x'
    || c == '<' || c == '>' || c == '^' || c == 'v')
  {
    return true;
  }
  else
  {
    return false;
  }
  return true;
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c)
{
  switch (c)
  {
  case '^':
    return 'w';
  case 'v':
    return 's';
  case '<':
    return 'a';
  case '>':
    return 'd';
  default:
    exit(0);
  }
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c)
{
  switch (c)
  {
  case 'W':
    return '^';
  case 'A':
    return '<';
  case 'S':
    return 'v';
  case 'D':
    return '>';
  default:
    exit(0);
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
    } else {
        return cur_row;
    }
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
    } else {
        return cur_col;
    }
}

/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_state_t* state, unsigned int snum) {
  unsigned int col = state->snakes[snum].head_col;
  unsigned int row = state->snakes[snum].head_row;
  unsigned int next_row = get_next_row(row, state->board[row][col]);
  unsigned int next_col = get_next_col(col, state->board[row][col]);
  return state->board[next_row][next_col];
}

/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_state_t* state, unsigned int snum) {
  unsigned int col = state->snakes[snum].head_col;
  unsigned int row = state->snakes[snum].head_row;
  unsigned int next_col = get_next_col(col, state->board[row][col]);
  unsigned int next_row = get_next_row(row, state->board[row][col]);//get next row
  state->snakes[snum].head_col = next_col;// update col
  state->snakes[snum].head_row = next_row;//update row
  state->board[next_row][next_col] = state->board[row][col];//update current head
  state->board[row][col] = head_to_body(state->board[row][col]);//update pre_body
  return;
}

/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_state_t* state, unsigned int snum) {
  unsigned int col = state->snakes[snum].tail_col;
  unsigned int row = state->snakes[snum].tail_row;
  unsigned int next_row = get_next_row(row, state->board[row][col]);
  unsigned int next_col = get_next_col(col, state->board[row][col]);
  state->snakes[snum].tail_col = next_col;
  state->snakes[snum].tail_row = next_row;
  state->board[next_row][next_col] = body_to_tail(state->board[next_row][next_col]);
  state->board[row][col] = ' ';
  return;
}

/* Task 4.5 */
void update_state(game_state_t* state, int (*add_food)(game_state_t* state)) {
  for (int i = 0; i < state->num_snakes; i++)
  {
    char next_char = next_square(state, i);
    if (next_char == '#' || is_snake(next_char) && state->snakes[i].live == true)
    {
      state->board[state->snakes[i].head_row][state->snakes[i].head_col] = 'x';
      state->snakes[i].live = false;
    }
    else if (next_char == ' ' && state->snakes[i].live == true)
    {
      update_head(state, i);
      update_tail(state, i);
    }
    else if (next_char == '*' && state->snakes[i].live == true)
    {
      update_head(state, i);
      int res = add_food(state);
    }
    else
    {
      exit(0);
    }
  }
  return;
}

/* Task 5 */

game_state_t* load_board(FILE* fp) {
  game_state_t* loaded_state = (game_state_t*)malloc(sizeof(game_state_t));
  loaded_state->board = NULL;
  loaded_state->snakes = NULL;
  loaded_state->num_snakes = 0;
  unsigned int rows = 0;
  char buffer[100];
  while(fgets(buffer, sizeof(buffer), fp) != NULL){
    int current_cols = strcspn(buffer, "\n");
    char *current_row = (char *)malloc((current_cols+1) * sizeof(char));
    strncpy(current_row, buffer, current_cols);
    current_row[current_cols] = '\0'; // 添加字符串终止符
    loaded_state->board = (char**)realloc(loaded_state->board, (rows + 1) * sizeof(char *));
    loaded_state->board[rows] = current_row;
    rows++;
  }      
  loaded_state->num_rows = rows;
  unsigned int r = loaded_state->num_rows;
  return loaded_state;
}
/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  return;
}

/* Task 6.2 */
game_state_t* initialize_snakes(game_state_t* state) {
  // TODO: Implement this function.
  return NULL;
}
