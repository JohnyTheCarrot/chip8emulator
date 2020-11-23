#pragma once

#include <stack>
#include <SDL.h>

extern uint8_t registers[0x10];
extern std::stack<uint16_t> subroutine_stack;

#pragma warning(push)
#pragma warning(disable: 4838)
// insert that table here
#pragma warning(pop)
extern uint8_t random_access_memory[4096];

extern uint8_t delay_timer;
extern uint16_t data_address;
extern uint16_t program_counter;


extern uint16_t keys;

extern uint64_t* screen_buffer;

extern SDL_Renderer* renderer;
extern SDL_Surface* surface;
extern SDL_Texture* tex;
