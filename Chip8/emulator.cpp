#include "emulator.h"
#include <iostream>
#include "graphics.h"

void read_rom_from_file(const char* path)
{
	std::basic_ifstream<uint8_t> infile(path, std::ios_base::binary);
	if (!infile.read(random_access_memory + 0x200, 0x1000 - 0x200))
	{
		if (!infile.eof())
		{
			std::cout << "error" << std::endl;
			exit(-1);
		}
	}
}

void clear_screen()
{
	for (size_t i = 0; i < 32; i++)
    {
        screen_buffer[i] = 0;
    }
}

void jump(short address)
{
	program_counter = address;
}

void subroutine(short address)
{
	subroutine_stack.push(program_counter);
	program_counter = address;
}

void return_subroutine()
{
	program_counter = subroutine_stack.top();
	subroutine_stack.pop();
}

void set_data_address(short address)
{
	data_address = address;
}

void add_to_data_address(uint8_t reg)
{
	data_address += registers[reg];
}

void skip_eq(uint8_t reg, uint8_t value)
{
	if (registers[reg] == value)
		program_counter += 2;
}

void skip_neq(uint8_t reg, uint8_t value)
{
	if (registers[reg] != value)
		program_counter += 2;
}

void skip_reg_eq(uint8_t x, uint8_t y)
{
	if (registers[x] == registers[y])
		program_counter += 2;
}

void set_register(uint8_t reg, uint8_t value)
{
	registers[reg] = value;
}

void add_to_register(uint8_t reg, uint8_t value)
{
	registers[reg] += value;
}

void copy_y_x(uint8_t x, uint8_t y)
{
	registers[x] = registers[y];
}

void logical_or(uint8_t x, uint8_t y)
{
	registers[x] |= registers[y];
}

void logical_and(uint8_t x, uint8_t y)
{
	registers[x] &= registers[y];
}

void logical_xor(uint8_t x, uint8_t y)
{
	registers[x] ^= registers[y];
}

void add_with_carry(uint8_t x, uint8_t y)
{
	bool will_overflow = registers[x] + registers[y] > 0xFF;
	registers[x] += registers[y];
	registers[0xF] = will_overflow;
}

void subtract(uint8_t x, uint8_t y, uint8_t store_in)
{
	registers[store_in] = registers[x] - registers[y];
	registers[0xF] = registers[x] > registers[y];
}

void shift_left(uint8_t x, uint8_t y)
{
	registers[x] = registers[y];
	registers[x] <<= 1;
}

void shift_right(uint8_t x, uint8_t y)
{
	registers[x] = registers[y];
	registers[x] >>= 1;
}

void get_delay_timer(uint8_t reg)
{
	registers[reg] = delay_timer;
}

void set_delay_timer(uint8_t reg)
{
	delay_timer = registers[reg];
}

void get_key(uint8_t reg)
{
	if (!keys)
		program_counter -= 2;
	else
	{
		for (size_t i = 0; i < sizeof(keys); i++)
		{
			if ((keys >> (sizeof(keys) - i)) & 1U)
			{
				registers[reg] = i;
				return;
			}
		}
	}
}

void skip_if_key(uint8_t reg)
{
	bool key = (keys >> registers[reg]) & 1U;
	if (key)
		program_counter += 2;
}

void skip_if_not_key(uint8_t reg)
{
	bool key = (keys >> registers[reg]) & 1U;
	if (!key)
		program_counter += 2;
}

void registers_to_ram(uint8_t n)
{
	if (n == 0)
	{
		random_access_memory[data_address] = registers[0];
		return;
	}
	// copy registers v0 -> vX into RAM
	memcpy(&random_access_memory[data_address], &registers, n);
}

void ram_to_registers(uint8_t n)
{
	if (n == 0)
	{
		registers[0] = random_access_memory[data_address];
		return;
	}
	memcpy(&registers, &random_access_memory[data_address], n);
}

void font(uint8_t reg)
{
	data_address = 5 * registers[reg];
}

void binary_coded_decimal_conversion(uint8_t reg)
{
	uint8_t value = registers[reg];
	random_access_memory[data_address] = (uint8_t) floor(value % 10);
	random_access_memory[data_address + 1] = (uint8_t)floor((value / 10) % 10);
	random_access_memory[data_address + 2] = (uint8_t)floor((value / 100) % 10);
}

void draw(uint8_t x_address, uint8_t y_address, uint8_t n_lines) {
	uint8_t sprite[15] = {};
	memcpy(&sprite, &random_access_memory[data_address], n_lines);

	const uint8_t coord_x = registers[x_address] % 64;
	const uint8_t coord_y = registers[y_address] % 32;

	registers[0xF] = 0;
	int y = coord_y;
	for(int i = 0; i < n_lines; i++)
	{
		const uint64_t sprite_row = sprite[i];
		const uint64_t shifted_sprite_row = sprite_row << (64 - coord_x - 8);
		const uint64_t modified_row = screen_buffer[y] ^ shifted_sprite_row;
		// check if this XOR flipped any bits off, mark register 0xF as 1 otherwise
		if ((modified_row ^ screen_buffer[y]) == shifted_sprite_row)
			registers[0xF] = 1;
		// finally, update buffer[y]
		screen_buffer[y] = modified_row;
		y++;
	}
	render();
}

void handle_instruction(uint32_t instruction)
{
	//std::cout << std::hex << program_counter << " : " << (uint32_t)instruction << std::endl;
	const uint8_t opcode = (instruction & 0xF000) >> 12;
	const uint8_t vX_addr = (instruction & 0x0F00) >> 8;
	const uint8_t vY_addr = (instruction & 0x00F0) >> 4;
	const uint8_t N = instruction & 0x000F;
	const uint8_t NN = instruction & 0x00FF;
	const uint32_t NNN = instruction & 0x0FFF;
	switch (opcode)
	{
	case 0:
		switch (NNN)
		{
		case 0xE0:
			clear_screen();
			break;
		case 0xEE:
			return_subroutine();
			break;
		}
		break;
	case 1:
		jump(NNN);
		break;
	case 2:
		subroutine(NNN);
		break;
	case 3:
		skip_eq(vX_addr, NN);
		break;
	case 4:
		skip_neq(vX_addr, NN);
		break;
	case 5:
		skip_reg_eq(vX_addr, vY_addr);
		break;
	case 6:
		set_register(vX_addr, NN);
		break;
	case 7:
		add_to_register(vX_addr, NN);
		break;
	case 8:
		switch (N)
		{
		case 0:
			copy_y_x(vX_addr, vY_addr);
			break;
		case 1:
			logical_or(vX_addr, vY_addr);
			break;
		case 2:
			logical_and(vX_addr, vY_addr);
			break;
		case 3:
			logical_xor(vX_addr, vY_addr);
			break;
		case 4:
			add_with_carry(vX_addr, vY_addr);
			break;
		case 5:
			subtract(vX_addr, vY_addr, vX_addr);
			break;
		case 6:
			shift_right(vX_addr, vY_addr);
			break;
		case 7:
			subtract(vY_addr, vX_addr, vX_addr);
			break;
		case 0xE:
			shift_left(vX_addr, vY_addr);
			break;
			break;
		default:
			std::cout << std::hex << program_counter << " : " << (uint32_t)instruction << " <" << std::endl;
			break;
		}
		break;
	case 0xA:
		set_data_address(NNN);
		break;
	case 0xD:
		draw(vX_addr, vY_addr, N);
		break;
	case 0xE:
		switch (NN)
		{
		case 0x9E:
			skip_if_key(vX_addr);
			break;
		case 0xA1:
			skip_if_not_key(vX_addr);
			break;
		}
		break;
	case 0xF:
		switch (NN)
		{
		case 7:
			get_delay_timer(vX_addr);
			break;
		case 0x0A:
			get_key(vX_addr);
			break;
		case 0x15:
			set_delay_timer(vX_addr);
			break;
		case 0x1e:
			add_to_data_address(vX_addr);
			break;
		case 0x29:
			font(vX_addr);
			break;
		case 0x33:
			binary_coded_decimal_conversion(vX_addr);
			break;
		case 0x55:
			registers_to_ram(vX_addr);
			break;
		case 0x65:
			ram_to_registers(vX_addr);
			break;
		default:
			std::cout << std::hex << program_counter << " : " << (uint32_t)instruction << " <" << std::endl;
			break;
		}
		break;
	default:
		std::cout << std::hex << program_counter << " : " << (uint32_t)instruction << " <" << std::endl;
		break;
	}
}
