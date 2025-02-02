#include "../include/cpu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

int cpu_tick(cpu_chip8* cpu, unsigned int screen_width, unsigned int screen_height) {

  int inc_pc = 2;
  uint16_t opcode = cpu->memory[cpu->pc] << 8u | cpu->memory[cpu->pc + 1];


  switch(opcode&0xF000) {
    case 0x0000:
      if (opcode == 0x00E0) {
        memset(cpu->video, 0, sizeof(cpu->video));
      } else if (opcode == 0x00EE) {
        cpu->pc = cpu->stack[cpu->sp];
        cpu->sp -= 1;
        inc_pc = 0;
      } else {
        return -1;
      }
      break;
    case 0x1000:
      cpu->pc = opcode & 0x0FFF;
      inc_pc = 0;
      break;
    case 0x2000:
      cpu->sp += 1;
      cpu->stack[cpu->sp] = cpu->pc+2;
      cpu->pc = opcode&0xFFF;
      inc_pc = 0;
      break;
    case 0x3000:
      if (cpu->registers[((opcode&0xF00) >> 8)] == (opcode&0xFF)) {
        inc_pc += 2;
      }
      break;
    case 0x4000:
      if (cpu->registers[((opcode&0xF00) >> 8)] != (opcode&0xFF)) {
        inc_pc += 2;
      }
      break;
    case 0x5000:
      if (cpu->registers[((opcode&0xF00) >> 8)] == cpu->registers[((opcode&0xF0) >> 4)]){
        inc_pc += 2;
      }
      break;
    case 0x6000:
      cpu->registers[((opcode&0xF00) >> 8)] = opcode&0xFF;
      break;
    case 0x7000:
      cpu->registers[((opcode&0xF00) >> 8)] += opcode&0xFF;
      break;
    case 0x8000:
      if ((opcode&0xF) == 0x0) {
        cpu->registers[((opcode&0xF00)>>8)] = cpu->registers[((opcode&0xF0)>>4)];
      } else if((opcode&0xF) == 0x1) {
        cpu->registers[((opcode&0xF00)>>8)] = 
          cpu->registers[((opcode&0xF00)>>8)] | cpu->registers[((opcode&0xF0)>>4)];
      } else if((opcode&0xF) == 0x2) {
        cpu->registers[((opcode&0xF00)>>8)] = 
          cpu->registers[((opcode&0xF00)>>8)] & cpu->registers[((opcode&0xF0)>>4)];
      } else if((opcode&0xF) == 0x3) {
        cpu->registers[((opcode&0xF00)>>8)] = 
          cpu->registers[((opcode&0xF00)>>8)] ^ cpu->registers[((opcode&0xF0)>>4)];
      } else if((opcode&0xF) == 0x4) {
        uint16_t result = cpu->registers[((opcode&0xF00)>>8)] + cpu->registers[((opcode&0xF0)>>4)];
        if (result > 0xFF) {
          cpu->registers[0xF] = 0x1;
        } else {
          cpu->registers[0xF] = 0x0;
        }
        cpu->registers[((opcode&0xF00)>>8)] = (uint8_t)(result&0xFF);
      } else if((opcode&0xF) == 0x5) {
        uint8_t result = cpu->registers[((opcode&0xF00)>>8)] - cpu->registers[((opcode&0xF0)>>4)];
        if (cpu->registers[((opcode&0xF00)>>8)] >= cpu->registers[((opcode&0xF0)>>4)]) {
          cpu->registers[0xF] = 0x1;
        } else {
          cpu->registers[0xF] = 0x0;
        }
        cpu->registers[((opcode&0xF00)>>8)] -= cpu->registers[((opcode&0xF0)>>4)];
      } else if((opcode&0xF) == 0x6) {
        uint8_t result = cpu->registers[((opcode&0xF00)>>8)] & 0x1;
        cpu->registers[0xF] = result;
        cpu->registers[((opcode&0xF00)>>8)] >>= 1;
      } else if((opcode&0xF) == 0x7) {
        uint16_t result = cpu->registers[((opcode&0xF0)>>4)] - cpu->registers[((opcode&0xF00)>>8)];
        if (cpu->registers[((opcode&0xF0)>>4)] >= cpu->registers[((opcode&0xF00)>>8)]) {
          cpu->registers[0xF] = 0x1;
        } else {
          cpu->registers[0xF] = 0x0;
        }
        cpu->registers[((opcode&0xF00)>>8)] = (uint8_t)(result);
     } else if((opcode&0xF) == 0xE) {
        uint8_t result = (uint8_t)(cpu->registers[((opcode&0xF00)>>8)] & 0x80) >> 7;
        if (result == 0x1) {
          cpu->registers[0xF] = 0x1;
        } else {
          cpu->registers[0xF] = 0x0;
        }
        cpu->registers[((opcode&0xF00)>>8)] <<= 1;
      } else {
        return -1;
      }
      break;
    case 0x9000:
      if ((opcode & 0xF) != 0) {
        return -1;
      }
      if (cpu->registers[((opcode&0xF00)>>8)] != cpu->registers[((opcode&0xF0)>>4)]) {
        inc_pc += 2;
      }
      break;
    case 0xA000:
      cpu->index_register = (opcode&0xFFF);
      break;
    case 0xB000:
      cpu->pc = ((opcode&0xFFF)) + cpu->registers[0];
      inc_pc = 0;
      break;
    case 0xC000: {
      uint8_t random_byte = rand() & 0xFF;
      cpu->registers[((opcode&0xF00)>>8)] = ((opcode&0xFF) & random_byte);
    }
    break; 
    case 0xD000: {
      uint8_t x = ((opcode&0xF00) >> 8);
      uint8_t y = ((opcode&0xF0) >> 4);
      uint8_t n = opcode&0xF;


      uint8_t x_wrap = cpu->registers[x] % screen_width;
      uint8_t y_wrap = cpu->registers[y] % screen_height;

      cpu->registers[0xF] = 0;

      for (unsigned int i = 0; i < n; i++) {
        uint8_t bytes = cpu->memory[cpu->index_register+i];

        for (unsigned int j = 0; j < 8; j++) {
          uint8_t pixel = (bytes) & (0b10000000 >> j);
          uint8_t* screen_pixel = &cpu->video[(screen_width * (y_wrap+i)) + (x_wrap+j)];
          if (pixel != 0x0) {
            if (*screen_pixel != 0x0) {
              cpu->registers[0xF] = 1;
            }
            *screen_pixel ^= 0x1;
          }
        }
      }
    }
    break;
    case 0xE000: {
      if ((opcode & 0xFF) == 0x9E) {
        if (cpu->input_keys[cpu->registers[(opcode&0xF00)>>8]] == 0x1) {
          inc_pc += 2;
        }
      } else if ((opcode & 0xFF) == 0xA1) {
        if (cpu->input_keys[cpu->registers[(opcode&0xF00)>>8]] == 0x0) {
          inc_pc += 2;
        }
      }
    }
    break;
    case 0xF000 : {
      if ((opcode & 0xFF) == 0x07) {
        cpu->registers[(opcode&0xF00)>>8] = cpu->delay_timer;
      } else if ((opcode & 0xFF) == 0x0A) {
        int pressed = 0;
        for (uint8_t i = 0; i < 0x10; i++) {
          if (cpu->input_keys[i] == 0x1) {
            cpu->registers[(opcode & 0xF00) >> 8] = i;
            pressed = 1;
          }
        }
        if (pressed == 0) {
          inc_pc = 0;
        }
      } else if ((opcode & 0xFF) == 0x15) {
        cpu->delay_timer = cpu->registers[(opcode & 0xF00) >> 8];
      } else if ((opcode & 0xFF) == 0x18) {
        cpu->sound_timer = cpu->registers[(opcode & 0xF00) >> 8];
      } else if ((opcode & 0xFF) == 0x1E) {
        cpu->index_register = cpu->index_register + cpu->registers[(opcode&0xF00)>>8];
        cpu->index_register &= 0xFFF;
      } else if ((opcode & 0xFF) == 0x29) {
        cpu->index_register = 0x0 + (((opcode & 0xF00)>>8) * 5);
      } else if ((opcode & 0xFF) == 0x33) {
        uint8_t value = cpu->registers[(opcode&0xF00)>>8];
        if (value > 100) {
          cpu->memory[cpu->index_register] = (uint8_t)((value / 100));
        } else {
          cpu->memory[cpu->index_register] = 0x0;
        }
        if (value > 10) {
          cpu->memory[cpu->index_register+1] = (uint8_t)((value / 10) % 10);
        } else {
          cpu->memory[cpu->index_register+1] = 0x0;
        }
        if (value>0) {
          cpu->memory[cpu->index_register+2] = (uint8_t)((value%10));
        } else {
          cpu->memory[cpu->index_register+2] = 0x0;
        }
      } else if ((opcode & 0xFF) == 0x55) {
        uint8_t x = (opcode & 0x0F00) >> 8;

        for (int i = 0; i <= x; i++) {
          cpu->memory[cpu->index_register + i] = cpu->registers[i];
        }
      } else if ((opcode & 0xFF) == 0x65) {
        uint8_t x = (opcode & 0xF00) >> 8;

        for (int i = 0; i <= x; i++) {
          cpu->registers[i] = cpu->memory[cpu->index_register + i];
        }
      }
    }
    break;
    default:
      printf("Unrecognised Opcode: %x!\n", opcode);
      return -1;
  }

  cpu->pc += inc_pc;

  return 0;
}

int create_cpu(cpu_chip8** cpu) {
  *cpu = malloc(sizeof(cpu_chip8));

  if (!*cpu) {
    printf("Failed to allocate memory for cpu!");
    return -1;
  }

  memset(*cpu, 0, sizeof(cpu_chip8));


  //LOAD FONTS

  uint8_t fontset[80] =
  {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
  };

  for (unsigned int i = 0; i < 80; i++) {
    (*cpu)->memory[0x00 + i] = fontset[i];
  }

  printf("Chip 8 CPU sucessfully initialised!\n");
  return 0;
}

int cpu_load_rom(cpu_chip8* cpu, const char* rom) {
  
  FILE* rom_file;

  rom_file = fopen(rom, "rb");

  if (rom_file == NULL) {
    printf("Error reading from rom file!");
  }

  printf("Rom file found!\nReading...\n");
  
  unsigned int index = 0;
  while (1) {
    int byte = fgetc(rom_file);
    if (byte == -1) {
      break;
    }
    cpu->memory[0x200 + index] = (uint8_t)byte;
    index += 1;
  }
  
  printf("Rom successfully loaded into memory!\n");

  cpu->pc = 0x200;

  return 0;
}
