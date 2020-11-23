#pragma once
#include <fstream>
#include "emulatordata.h"

void read_rom_from_file(const char* path);

void handle_instruction(uint32_t instruction);