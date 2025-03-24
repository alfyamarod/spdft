#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <sys/mman.h>

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "spike.h"
#include "utils.h"

#define BYTES_PER_PIXEL 4
#define WIDTH 640
#define HEIGHT 400
#define DEBUG_MODE 1
#define DEBUG_FILE "error_log.txt"

FILE *error_log;

GLOBAL_VAR bool Running = true;
GLOBAL_VAR SDL_Renderer *renderer = NULL;

typedef struct {
  SDL_Texture *texture;
  void *pixels;
  u32 _width;
  u32 _height;
} offscreen_buffer;

GLOBAL_VAR offscreen_buffer ofs_buff = {0};

static void render_gradient(int xofset, int yofset) {

  // on little endian AARRGGBB

  u32 pitch = ofs_buff._width * BYTES_PER_PIXEL;
  u8 *row = (u8 *)ofs_buff.pixels;

  for (int i = 0; i < ofs_buff._height; i++) {
    u32 *pixel = (u32 *)row;
    for (int j = 0; j < ofs_buff._width; j++) {
      u8 blue = j + xofset;
      u8 green = i + yofset;

      *pixel++ = ((green << 8) | blue);
    }
    row += pitch;
  }
}

static void resize_texture(int width, int height) {

  if (ofs_buff.texture) {
    SDL_DestroyTexture(ofs_buff.texture);
  }

  if (ofs_buff.pixels) {
    StopIf(munmap(ofs_buff.pixels,
                  ofs_buff._width * ofs_buff._height * BYTES_PER_PIXEL) == -1,
           exit(EXIT_FAILURE), "munmap failed");
  }

  ofs_buff.pixels =
      mmap(0, BYTES_PER_PIXEL * width * height, PROT_READ | PROT_WRITE,
           MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

  StopIf(ofs_buff.pixels == MAP_FAILED, exit(EXIT_FAILURE), "mmap() failed");

  ofs_buff.texture =
      SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                        SDL_TEXTUREACCESS_STREAMING, width, height);

  StopIf(!ofs_buff.texture, exit(EXIT_FAILURE), "Unable to create texture %s ",
         SDL_GetError());

  // IMPORTANT global size variables at this point need to be set
  ofs_buff._width = width;
  ofs_buff._height = height;

  render_gradient(width, height);
}

static void update_window() {

  // TODO change this to lock texture (check what pixels are at this point)

  StopIf(SDL_UpdateTexture(ofs_buff.texture, NULL, ofs_buff.pixels,
                           ofs_buff._width * BYTES_PER_PIXEL) == false,
         exit(EXIT_FAILURE), "Failed to update texture %s\n", SDL_GetError());

  /* int pitch = texture_width * BYTES_PER_PIXEL; */

  /* void *temp_pixels; */

  /* StopIf(SDL_LockTexture(texture, NULL, &temp_pixels, &pitch) == false, */
  /*        exit(EXIT_FAILURE), "Failed to lock in texture: %s\n",
   * SDL_GetError()); */

  /* memcpy(temp_pixels, pixels, pitch); */
  /* u8 *temp = (u8 *)pixels; */
  /* printf("%u\n", *temp); */

  /* SDL_UnlockTexture(texture); */

  StopIf(SDL_RenderTexture(renderer, ofs_buff.texture, NULL, NULL) == false,
         exit(EXIT_FAILURE), "Failed to render texture %s\n", SDL_GetError());
}

bool handle_event(SDL_Event *event) {

  SDL_Window *window = SDL_GetWindowFromID(event->window.windowID);
  SDL_Renderer *renderer = SDL_GetRenderer(window);

  switch (event->type) {
  case SDL_EVENT_QUIT: {
    return true;
  } break;
  case SDL_EVENT_WINDOW_RESIZED: {
    int width, height;
    SDL_GetWindowSize(window, &width, &height);
    resize_texture(width, height);

  } break;
  case SDL_EVENT_WINDOW_EXPOSED: {
    update_window();
  }

  break;
  default:
    // TODO
    break;
  }
  return false;
}

int main(int argc, char *argv[]) {

  bool gui = true;

  printf("*** Spike DFT ***");

  if (DEBUG_MODE == 1) {
    error_log = fopen(DEBUG_FILE, "w");
  }

  if (!gui) {

    u32 num_pops = 3;

    spk_params_t params = {.num_pops = num_pops, .populations = NULL};

    spk_set_sim(&params);

    exit(EXIT_SUCCESS);
  }

  // TODO make it multiprocess one for simulation and remaining for simulation

  SDL_Window *window = NULL;

  SDL_Init(SDL_INIT_VIDEO);

  bool w_r_created = SDL_CreateWindowAndRenderer(
      "Hello SDL", WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer);

  StopIf(w_r_created == false, exit(EXIT_FAILURE),
         "Could not create window %s\n", SDL_GetError());

  ofs_buff.texture =
      SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                        SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

  ofs_buff._height = HEIGHT;
  ofs_buff._width = WIDTH;

  StopIf(!ofs_buff.texture, exit(EXIT_FAILURE), "Could not create texture %s\n",
         SDL_GetError());

  ofs_buff.pixels =
      mmap(0, BYTES_PER_PIXEL * WIDTH * HEIGHT, PROT_READ | PROT_WRITE,
           MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

  resize_texture(WIDTH, HEIGHT);

  while (Running) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      if (handle_event(&event)) {
        Running = false;
      }
    }

    // Logic

    SDL_RenderClear(renderer);
    update_window();

    SDL_RenderPresent(renderer);
  }

  SDL_Quit();

  if (DEBUG_MODE) {
    fclose(error_log);
  }
}
