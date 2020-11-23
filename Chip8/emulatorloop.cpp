#include "emulatorloop.h"
#include "emulatordata.h"
#include "emulator.h"
#include <iostream>

void set_key(uint8_t chip8_key_code, bool state)
{
	keys ^= (-state ^ keys) & (1UL << chip8_key_code);
}

void handle_key(SDL_Scancode scancode, bool down)
{
	switch (scancode)
	{
	case SDL_SCANCODE_Q:
		set_key(0, down);
		break;
	case SDL_SCANCODE_W:
		set_key(1, down);
		break;
	case SDL_SCANCODE_E:
		set_key(2, down);
		break;
	case SDL_SCANCODE_R:
		set_key(3, down);
		break;
	case SDL_SCANCODE_A:
		set_key(4, down);
		break;
	case SDL_SCANCODE_S:
		set_key(5, down);
		break;
	case SDL_SCANCODE_D:
		set_key(6, down);
		break;
	case SDL_SCANCODE_F:
		set_key(7, down);
		break;
	case SDL_SCANCODE_Z:
		set_key(8, down);
		break;
	case SDL_SCANCODE_X:
		set_key(9, down);
		break;
	case SDL_SCANCODE_C:
		set_key(10, down);
		break;
	case SDL_SCANCODE_V:
		set_key(11, down);
		break;
	}
	if (keys)
		std::cout << keys << std::endl;
}

void handle_events(bool* shouldClose)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			*shouldClose = true;
			break;
		case SDL_KEYDOWN:
			handle_key(event.key.keysym.scancode, true);
			break;
		case SDL_KEYUP:
			handle_key(event.key.keysym.scancode, false);
			break;
		}
	}
}

uint32_t get_instruction()
{
	uint8_t char_0 = random_access_memory[program_counter];
	uint8_t char_1 = random_access_memory[program_counter + 1];

	return (char_0 << 8) | char_1;
}

void emulator_tick()
{
	uint32_t instruction = get_instruction();
	program_counter += 2;
	if (delay_timer > 0)
		delay_timer--;
	handle_instruction(instruction);
}
