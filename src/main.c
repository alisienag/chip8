#include <SDL3/SDL_events.h>
#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../include/cpu.h"

#include "SDL3/SDL.h"

int main(int argc, char** args) {
  if (argc <= 1) {
    printf("Error, no rom given. Instructions: chip8 *rom_name*\n");
    return -1;
  }
  printf("%s\n", args[1]);

  char cwd[4096];

  if(getcwd(cwd, sizeof(cwd)) == NULL) {
    printf("Error finding current working directory!");
    return -1;
  }
  strcat(cwd, "/");
  strcat(cwd, args[1]);

  printf("Loading from from: %s\n", cwd);

  cpu_chip8* cpu;
  create_cpu(&cpu);
  cpu_load_rom(cpu, cwd);

  printf("Creating window!\n");

  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
  SDL_Window* window;
  SDL_Renderer* renderer;

  unsigned int screen_width = 64;
  unsigned int screen_height = 32;
  unsigned int screen_scale = 10;


  window = SDL_CreateWindow("Chip8 Emulator", screen_width*screen_scale, screen_height * screen_scale, SDL_WINDOW_OPENGL);
  int isRunning = 1;
  
  if (window == NULL) {
    printf("Error creating window!");
    return -1;
  }

  printf("Created Window!\n");

  SDL_SetWindowResizable(window, 1);

  renderer = SDL_CreateRenderer(window, NULL);

  if (renderer == NULL) {
    printf("Renderer failed!\n");
    return -1;
  }


  SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, screen_width, screen_height);
  SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
  uint32_t* pixelBuffer = (uint32_t*)malloc(screen_width * screen_height * sizeof(uint32_t));

  int last_tick_time = SDL_GetTicks();

  float one_second_timer = 0;

  const int instructions_per_frame = 10;
  const float frame_delay = 1000.f/60.f;

  while(isRunning == 1) {
    int frame_start = SDL_GetTicks();
    SDL_RenderClear(renderer);

    if (cpu->sound_timer > 0) {
      cpu->sound_timer -= 1;
    }
    if (cpu->delay_timer > 0) {
      cpu->delay_timer -= 1;
    }
    for (int tick_interval = 0; tick_interval < instructions_per_frame; tick_interval++) {
      if(cpu_tick(cpu, screen_width, screen_height) == -1) {
        isRunning = 0;
      }
    }
    for (int y = 0; y < screen_height; y++) {
        for (int x = 0; x < screen_width; x++) {
            uint8_t pixel = cpu->video[(y * screen_width) + x];
            if (pixel != 0x0) {
                pixelBuffer[(y * screen_width) + x] = 0xFFFFFFFF;
            } else {
                pixelBuffer[(y * screen_width) + x] = 0x00000000;
            }
        }
    }
    SDL_UpdateTexture(texture, NULL, pixelBuffer, (screen_width) * sizeof(uint32_t));
    SDL_RenderTexture(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if(event.type == SDL_EVENT_QUIT) {
        isRunning = 0;
      }
      switch (event.type) {
        case SDL_EVENT_KEY_DOWN:
          switch (event.key.key) {
            case SDLK_ESCAPE:
              isRunning = 0;
              break;
            case SDLK_X:
              cpu->input_keys[0] = 1;
              break;
            case SDLK_1:
              cpu->input_keys[1] = 1;
              break;
            case SDLK_2:
              cpu->input_keys[2] = 1;
              break;
            case SDLK_3:
              cpu->input_keys[3] = 1;
              break;
            case SDLK_Q:
              cpu->input_keys[4] = 1;
              break;
            case SDLK_W:
              cpu->input_keys[5] = 1;
              break;
            case SDLK_E:
              cpu->input_keys[6] = 1;
              break;
            case SDLK_A:
              cpu->input_keys[7] = 1;
              break;
            case SDLK_S:
              cpu->input_keys[8] = 1;
              break;
            case SDLK_D:
              cpu->input_keys[9] = 1;
              break;
            case SDLK_Z:
              cpu->input_keys[10] = 1;
              break;
            case SDLK_C:
              cpu->input_keys[11] = 1;
              break;
            case SDLK_4:
              cpu->input_keys[12] = 1;
              break;
            case SDLK_R:
              cpu->input_keys[13] = 1;
              break;
            case SDLK_F:
              cpu->input_keys[14] = 1;
              break;
            case SDLK_V:
              cpu->input_keys[15] = 1;
              break;
          }
        break;
        case SDL_EVENT_KEY_UP:
          switch (event.key.key) {
            case SDLK_ESCAPE:
              isRunning = 0;
              break;
            case SDLK_X:
              cpu->input_keys[0] = 0;
              break;
            case SDLK_1:
              cpu->input_keys[1] = 0;
              break;
            case SDLK_2:
              cpu->input_keys[2] = 0;
              break;
            case SDLK_3:
              cpu->input_keys[3] = 0;
              break;
            case SDLK_Q:
              cpu->input_keys[4] = 0;
              break;
            case SDLK_W:
              cpu->input_keys[5] = 0;
              break;
            case SDLK_E:
              cpu->input_keys[6] = 0;
              break;
            case SDLK_A:
              cpu->input_keys[7] = 0;
              break;
            case SDLK_S:
              cpu->input_keys[8] = 0;
              break;
            case SDLK_D:
              cpu->input_keys[9] = 0;
              break;
            case SDLK_Z:
              cpu->input_keys[10] = 0;
              break;
            case SDLK_C:
              cpu->input_keys[11] = 0;
              break;
            case SDLK_4:
              cpu->input_keys[12] = 0;
              break;
            case SDLK_R:
              cpu->input_keys[13] = 0;
              break;
            case SDLK_F:
              cpu->input_keys[14] = 0;
              break;
            case SDLK_V:
              cpu->input_keys[15] = 0;
              break;
          }
      } 
    }
   int frame_time = SDL_GetTicks() - frame_start;
    if (frame_delay > frame_time) {
      SDL_Delay(frame_delay - frame_time);
    }
  }
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  free(pixelBuffer);

  SDL_Quit();
  return 0;
}


