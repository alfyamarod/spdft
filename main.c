#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <sys/mman.h>

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
GLOBAL_VAR SDL_Texture *texture = NULL;
GLOBAL_VAR void *pixels = NULL;
GLOBAL_VAR SDL_Renderer *renderer = NULL;
GLOBAL_VAR int texture_width = WIDTH;
GLOBAL_VAR int texture_height = HEIGHT;

static void resize_texture(int width, int height) {

  if (texture) {
    SDL_DestroyTexture(texture);
  }

  if (pixels) {
    munmap(pixels, width * height * BYTES_PER_PIXEL);
  }

  pixels = mmap(0, BYTES_PER_PIXEL * WIDTH * HEIGHT, PROT_READ | PROT_WRITE,
                MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                              SDL_TEXTUREACCESS_STREAMING, width, height);

  StopIf(!texture, exit(EXIT_FAILURE), "Unable to create texture %s ",
         SDL_GetError());

  /* uint32_t pitch = width * BYTES_PER_PIXEL; */
  /* uint8_t *row = (uint8_t *)pixels; */

  /* for (int i = 0; i < height; i++) { */
  /*   uint8_t *pixel = (uint8_t *)row; */
  /*   for (int j = 0; j < width; j++) { */
  /*     *pixel = 255; */
  /*     ++pixel; */
  /*     *pixel = 0; */
  /*     ++pixel; */
  /*     *pixel = 0; */
  /*     ++pixel; */
  /*   } */
  /*   row += pitch; */
  /* } */
}

static void update_window() {

  StopIf(SDL_UpdateTexture(texture, NULL, pixels,
                           texture_width * BYTES_PER_PIXEL) == false,
         exit(EXIT_FAILURE), "Failed to update texture %s\n", SDL_GetError());

  StopIf(SDL_RenderTexture(renderer, texture, NULL, NULL) == false,
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

    printf("Resizing %d %d\n", width, height);
    resize_texture(width, height);
    texture_width = width;
    texture_height = height;

  } break;
  case SDL_EVENT_WINDOW_EXPOSED: {
    printf("Updating window\n");
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

  printf(" *** Spike DFT ***");

  if (DEBUG_MODE == 1) {
    error_log = fopen(DEBUG_FILE, "w");
  }

  SDL_Window *window = NULL;

  SDL_Init(SDL_INIT_VIDEO);
  window = SDL_CreateWindow("SPDFT", WIDTH, HEIGHT,
                            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
  StopIf(!window, exit(EXIT_FAILURE), "Could not create window %s\n",
         SDL_GetError());

  renderer = SDL_CreateRenderer(window, NULL);

  SDL_PropertiesID prop = SDL_GetRendererProperties(renderer);

  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                              SDL_TEXTUREACCESS_STREAMING, texture_width,
                              texture_height);
  StopIf(!texture, exit(EXIT_FAILURE), "Could not create texture %s\n",
         SDL_GetError());

  pixels = mmap(0, BYTES_PER_PIXEL * texture_width * texture_height,
                PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

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
    SDL_RenderPresent(renderer);
  }

  SDL_Quit();

  if (DEBUG_MODE) {
    fclose(error_log);
  }
}
