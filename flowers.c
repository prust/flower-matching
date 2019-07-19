#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>

#include "SDL.h"
#include "SDL_image.h"
#include "font8x8_basic.h"

#define NONE -1

typedef unsigned char byte;

typedef struct {
  int x;
  int y;
  int w;
  int h;
} Viewport;

typedef struct {
  int curr_tile_pos;
  int sel_tile_pos;
  int coins;
} Player;

int to_x(int ix);
int to_y(int ix);
int to_pos(int x, int y);
int left(int ix);
int right(int ix);
int above(int ix);
int below(int ix);
bool is_in_grid(int x, int y);

int render_text(SDL_Renderer* renderer, char str[], int offset_x, int offset_y, int size);
void render_sprite(SDL_Renderer* renderer, SDL_Texture* spritesheet, byte sprite_id, int dest_x, int dest_y);

void on_select_curr_tile(Player* player, int* curr_player_turn, byte sprites[]);
bool check_for_point(byte sprites[], int pos, Player* player);
void replace_flowers(byte sprites[], int target_sprite_id, int start_pos, int end_pos);

void error(char* activity);

int num_blocks_w = 8;
int num_blocks_h = 8;
int grid_len;

int target_matches = 4;
int max_moves = 1;
int moves_left = 1;
int num_passes = 0;

int block_w = 100;
int block_h = 100;
int grid_spacing = 6;
Viewport vp = {.x = 0, .y = 0};

int curr_player_turn = 1;

Player player1 = {
  .curr_tile_pos = 0,
  .sel_tile_pos = NONE,
  .coins = 0
};

Player player2 = {
  .curr_tile_pos = 0,
  .sel_tile_pos = NONE,
  .coins = 0
};

int hud_height = 100;

int main(int num_args, char* args[]) {
  srand(time(NULL));
  grid_len = num_blocks_w * num_blocks_h;
  byte grid_flags[grid_len];
  byte sprites[grid_len];

  // manually init b/c C doesn't allow initializing VLAs w/ {0}
  for (int i = 0; i < grid_len; ++i) {
    grid_flags[i] = 0;
    sprites[i] = rand() % 9;
  }


  // SDL setup
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
    error("initializing SDL");

  SDL_Window* window;
  window = SDL_CreateWindow("Jessica's Flower Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, num_blocks_w * block_w, num_blocks_h * block_h, SDL_WINDOW_RESIZABLE);
  if (!window)
    error("creating window");
  
  // toggle_fullscreen(window);
  SDL_GetWindowSize(window, &vp.w, &vp.h);

  // block_h = (vp.h - hud_height) / 15 - grid_spacing;
  // block_w = vp.w / 30 - grid_spacing;

  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer)
    error("creating renderer");

  if (SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND) < 0)
    error("setting blend mode");

  SDL_Texture* spritesheet = IMG_LoadTexture(renderer, "spritesheet.png");
  if (!spritesheet)
    error("loading spritesheet");

  SDL_Event evt;
  bool exit_game = false;
  unsigned int last_time = -1;
  unsigned int curr_time = -1;
  while (!exit_game) {
    last_time = curr_time;
    curr_time = SDL_GetTicks();

    // process input events
    while (SDL_PollEvent(&evt)) {
      if (evt.type == SDL_QUIT || (evt.type == SDL_KEYDOWN && evt.key.keysym.sym == SDLK_ESCAPE)) {
        exit_game = true;
      }
      else if (evt.type == SDL_MOUSEBUTTONDOWN) {
        if (curr_player_turn == 1)
          on_select_curr_tile(&player1, &curr_player_turn, sprites);

        // if (grid_flags[pos] & EXPLORED) {
        //   if (terrain_id == RCK)
        //     player1.metal++;
        //   else if (terrain_id == GRS)
        //     player1.wood++;
        // }
        // else {
        //   int exploration_cost = 2;
        //   if (player1.coins >= exploration_cost) {
        //     grid_flags[pos] |= EXPLORED;
        //     tile_init[pos] = curr_time;
        //     player1.coins -= exploration_cost;
        //   }
        // }
      }
      // else if (evt.type == SDL_WINDOWEVENT && evt.window.event == SDL_WINDOWEVENT_RESIZED) {
      //   SDL_GetWindowSize(window, &vp.w, &vp.h);
      //   block_h = (vp.h - hud_height) / 15 - grid_spacing;
      //   block_w = vp.w / 30 - grid_spacing;
      // }
      else if (evt.type == SDL_MOUSEMOTION) {
        int x = evt.motion.x / (block_w + grid_spacing);
        int y = evt.motion.y / (block_h + grid_spacing);
        if (x < num_blocks_w && y < num_blocks_h)
          player1.curr_tile_pos = to_pos(x, y);
      }
      else if (evt.type == SDL_KEYDOWN) {
        if (evt.key.keysym.sym == SDLK_LEFT)
          player2.curr_tile_pos = left(player2.curr_tile_pos);
        else if (evt.key.keysym.sym == SDLK_RIGHT)
          player2.curr_tile_pos = right(player2.curr_tile_pos);
        else if (evt.key.keysym.sym == SDLK_UP)
          player2.curr_tile_pos = above(player2.curr_tile_pos);
        else if (evt.key.keysym.sym == SDLK_DOWN)
          player2.curr_tile_pos = below(player2.curr_tile_pos);
        else if (evt.key.keysym.sym == SDLK_SPACE && curr_player_turn == 2)
          on_select_curr_tile(&player2, &curr_player_turn, sprites);
        else if (evt.key.keysym.sym == SDLK_p) {
          num_passes++;
          
          // every time both players pass in a row, bump up the max number of moves
          if (num_passes % 2 == 0)
            max_moves++;

          moves_left = max_moves;
          if (curr_player_turn == 1)
            curr_player_turn = 2;
          else
            curr_player_turn = 1;
        }
      }
    }

    // set BG color
    if (SDL_SetRenderDrawColor(renderer, 40, 23, 10, 255) < 0)
      error("setting bg color");
    if (SDL_RenderClear(renderer) < 0)
      error("clearing renderer");
    
    int mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);


    // render
    for (int y = 0; y < num_blocks_h - 1; y++) {
      for (int x = 0; x < num_blocks_w - 1; x++) {
        int pos = to_pos(x, y);
        
        if (SDL_SetRenderDrawColor(renderer, 40, 23, 10, 255) < 0)
          error("setting tile background color");

        // Draw Tile
        SDL_Rect terrain_rect = {
          .x = x * (block_w + grid_spacing),
          .y = y * (block_h + grid_spacing),
          .w = block_w,
          .h = block_h
        };
        
        if (SDL_RenderFillRect(renderer, &terrain_rect) < 0)
          error("filling terrain rect");

        // render sprite
        render_sprite(renderer, spritesheet, sprites[pos], x, y);

        // draw selection box
        if ((curr_player_turn == 1 && (pos == player1.curr_tile_pos || pos == player1.sel_tile_pos)) || (curr_player_turn == 2 && (pos == player2.curr_tile_pos  || pos == player2.sel_tile_pos))) {
          if (pos == player1.sel_tile_pos || pos == player2.sel_tile_pos) {
            if (SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255) < 0)
              error("setting selection color");
          }
          else if (pos == player2.curr_tile_pos) {
            if (SDL_SetRenderDrawColor(renderer, 253, 80, 80, 255) < 0)
              error("setting player 2 curr-tile color");
          }
          else {
            if (SDL_SetRenderDrawColor(renderer, 71, 113, 255, 255) < 0)
              error("setting player 1 curr-tile color");
          }
          
          SDL_Rect sel_rect = {
            .x = x * (block_w + grid_spacing) - 2,
            .y = y * (block_h + grid_spacing) - 2,
            .w = block_w + 4,
            .h = block_h + 4
          };
          SDL_RenderDrawRect(renderer, &sel_rect);

          // draw 2px rect by drawing a 2nd time, but inside
          sel_rect.x++;
          sel_rect.y++;
          sel_rect.w -= 2;
          sel_rect.h -= 2;
          SDL_RenderDrawRect(renderer, &sel_rect);
        }
      }
    }

    char resources_str[80];
    snprintf(resources_str, sizeof(resources_str), "Player 1: %d, Player 2: %d    Player %d: %d Moves Left", player1.coins, player2.coins, curr_player_turn, moves_left);
    if (SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255) < 0)
      error("setting text color");

    int padding = 5;
    render_text(renderer, resources_str, padding, (num_blocks_h - 1) * (block_h + grid_spacing) + padding, 2);

    SDL_RenderPresent(renderer);
    SDL_Delay(10);
  }
  
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}


// game logic functions

void on_select_curr_tile(Player* player, int* curr_player_turn, byte sprites[]) {
  if (player->sel_tile_pos == NONE) {
    player->sel_tile_pos = player->curr_tile_pos;
  }
  else {
    if (player->curr_tile_pos == player->sel_tile_pos) {
      player->sel_tile_pos = NONE;
    }
    else {
      // Check if adjacent (or adjacent-diagonal)
      int curr_x = to_x(player->curr_tile_pos);
      int curr_y = to_y(player->curr_tile_pos);
      int sel_x = to_x(player->sel_tile_pos);
      int sel_y = to_y(player->sel_tile_pos);

      if (abs(curr_x - sel_x) <= 1 && abs(curr_y - sel_y) <= 1) {
        moves_left--;

        // Swap curr & selected tiles
        int curr_sprite_id = sprites[player->curr_tile_pos];
        sprites[player->curr_tile_pos] = sprites[player->sel_tile_pos];
        sprites[player->sel_tile_pos] = curr_sprite_id;

        bool sel_points = check_for_point(sprites, player->sel_tile_pos, player);
        bool curr_points = check_for_point(sprites, player->curr_tile_pos, player);

        // if the player ran out of moves & didn't make a point
        // swap the tiles back
        if (!sel_points && !curr_points && !moves_left) {
          sprites[player->sel_tile_pos] = sprites[player->curr_tile_pos];
          sprites[player->curr_tile_pos] = curr_sprite_id;
        }

        player->sel_tile_pos = NONE;

        // switch to the other player's turn
        if (!moves_left || sel_points || curr_points) {
          if (*curr_player_turn == 1)
            *curr_player_turn = 2;
          else
            *curr_player_turn = 1;

          // if there are left-over moves, reduce the number
          if (moves_left)
            max_moves = max_moves - moves_left;
          moves_left = max_moves;

          num_passes = 0;
        }     
      }
    }
  }
}

bool check_for_point(byte sprites[], int pos, Player* player) {
  bool got_points = false;
  int pos_x = to_x(pos);
  int pos_y = to_y(pos);

  int target_sprite_id = sprites[pos];
  int num_matches;

  // check for horiz match
  num_matches = 0;
  int start_x = pos_x - (target_matches - 1);
  if (start_x < 0)
    start_x = 0;
  int end_x = pos_x + (target_matches - 1);
  if (end_x > num_blocks_w - 1)
    end_x = num_blocks_w - 1;

  for (int x = start_x; x <= end_x; ++x) {
    if (sprites[to_pos(x, pos_y)] == target_sprite_id) {
      num_matches++;
    }
    else {
      if (num_matches >= target_matches) {
        player->coins++;
        got_points = true;
        replace_flowers(sprites, target_sprite_id, to_pos(x - 1, pos_y), to_pos(start_x, pos_y));
      }
      num_matches = 0;
    }
  }

  if (num_matches >= target_matches) {
    player->coins++;
    got_points = true;
    replace_flowers(sprites, target_sprite_id, to_pos(end_x, pos_y), to_pos(start_x, pos_y));
  }

  // check for vert match
  num_matches = 0;
  int start_y = pos_y - (target_matches - 1);
  if (start_y < 0)
    start_y = 0;
  int end_y = pos_y + (target_matches - 1);
  if (end_y > num_blocks_h - 1)
    end_y = num_blocks_h - 1;

  for (int y = start_y; y <= end_y; ++y) {
    if (sprites[to_pos(pos_x, y)] == target_sprite_id) {
      num_matches++;
    }
    else {
      if (num_matches >= target_matches) {
        player->coins++;
        got_points = true;
        replace_flowers(sprites, target_sprite_id, to_pos(pos_x, y - 1), to_pos(pos_x, start_y));
      }
      num_matches = 0;
    }
  }

  if (num_matches >= target_matches) {
    player->coins++;
    got_points = true;
    replace_flowers(sprites, target_sprite_id, to_pos(pos_x, end_y), to_pos(pos_x, start_y));
  }

  return got_points;
}

void replace_flowers(byte sprites[], int target_sprite_id, int start_pos, int end_pos) {
  int end_x = to_x(end_pos);
  int end_y = to_y(end_pos);

  int x = to_x(start_pos);
  int y = to_y(start_pos);
  
  // verify that we're working backwards (or staying the same) in each dimension
  if (end_x > x)
    error("end_x should be less than x");
  if (end_y > y)
    error("end_y should be less than y");

  // march backwards and replace all matching sprites
  while (x >= end_x && y >= end_y && sprites[to_pos(x, y)] == target_sprite_id) {
    sprites[to_pos(x, y)] = rand() % 9;

    if (x != end_x)
      x--;
    if (y != end_y)
      y--;
  }
}


// boilerplate logic

void render_sprite(SDL_Renderer* renderer, SDL_Texture* sprites, byte sprite_id, int dest_x, int dest_y) {
  int sprite_x = sprite_id % 3;
  int sprite_y = sprite_id / 3;
  SDL_Rect src = {.x = sprite_x * 100, .y = sprite_y * 100, .w = 100, .h = 100};
  SDL_Rect dest = {.x = dest_x * (block_w + grid_spacing) - vp.x, .y = dest_y * (block_h + grid_spacing) - vp.y, .w = block_w, .h = block_h};
  if (SDL_RenderCopy(renderer, sprites, &src, &dest) < 0)
    error("render_sprite");
}

int to_x(int ix) {
  return ix % num_blocks_w;
}

int to_y(int ix) {
  return ix / num_blocks_w;
}

int to_pos(int x, int y) {
  int pos = x + y * num_blocks_w;
  if (pos < 0 || pos >= grid_len)
    printf("x: %d, y: %d\n", x, y);
  if (pos < 0)
    error("position out of bounds (negative)");
  if (pos >= grid_len)
    error("position out of bounds (greater than grid size)");
  return pos;
}

int left(int ix) {
  int x = to_x(ix);
  int y = to_y(ix);
  return to_pos(x - 1, y);
}
int right(int ix) {
  int x = to_x(ix);
  int y = to_y(ix);
  return to_pos(x + 1, y);
}
int above(int ix) {
  int x = to_x(ix);
  int y = to_y(ix);
  return to_pos(x, y - 1);
}
int below(int ix) {
  int x = to_x(ix);
  int y = to_y(ix);
  return to_pos(x, y + 1);
}

bool is_in_grid(int x, int y) {
  if ((x < 0 || x >= num_blocks_w) ||
    (y < 0 || y >= num_blocks_h))
      return false;

  return true;
}

int render_text(SDL_Renderer* renderer, char str[], int offset_x, int offset_y, int size) {
  int i;
  for (i = 0; str[i] != '\0'; ++i) {
    int code = str[i];
    if (code < 0 || code > 127)
      error("Text code out of range");

    char* bitmap = font8x8_basic[code];
    int set = 0;
    for (int y = 0; y < 8; ++y) {
      for (int x = 0; x < 8; ++x) {
        set = bitmap[y] & 1 << x;
        if (!set)
          continue;

        SDL_Rect r = {
          .x = offset_x + i * (size) * 8 + x * size,
          .y = offset_y + y * size,
          .w = size,
          .h = size
        };
        if (SDL_RenderFillRect(renderer, &r) < 0)
          error("drawing text block");
      }
    }
  }

  // width of total text string
  return i * size * 8;
}

void error(char* activity) {
  printf("%s failed: %s\n", activity, SDL_GetError());
  SDL_Quit();
  exit(-1);
}