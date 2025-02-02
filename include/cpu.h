#include <stdint.h>
#include <stdio.h>

#define START_ADDRESS = 0x200;

typedef struct {
  uint8_t registers[16]; 
  uint8_t memory[4096];
  uint16_t index_register;
  uint16_t pc;
  uint16_t stack[16];
  uint8_t sp;
  uint8_t delay_timer;
  uint8_t sound_timer;
  uint8_t input_keys[16];
  uint8_t video[64*32];
  uint16_t opcode;
} cpu_chip8;


int create_cpu(cpu_chip8** cpu);
int cpu_load_rom(cpu_chip8* cpu, const char* rom);

int cpu_tick(cpu_chip8* cpu, unsigned int screen_width, unsigned int screen_height);
