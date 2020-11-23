#pragma once
#include <SDL.h>

void handle_events(bool* shouldClose);

uint32_t get_instruction();

void emulator_tick();