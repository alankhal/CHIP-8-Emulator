#pragma once

#include <cstdint>
#include <chrono>
#include <random>

const unsigned int VIDEO_WIDTH = 64;
const unsigned int VIDEO_HEIGHT = 32;
const unsigned int START_ADDRESS = 0x200;
const unsigned int FONTSET_START_ADDRESS = 0x50;
const unsigned int FONTSET_SIZE = 80;

class Chip8
{
public:
	Chip8();

	void LoadROM(char const* filename);
	void Cycle();
	void UpdateTimers();

	uint8_t keypad[16]{};
	uint32_t video[VIDEO_WIDTH * VIDEO_HEIGHT]{};

	uint8_t registers[16]{};
	uint8_t memory[4096]{};
	uint16_t index{};
	uint16_t pc{};
	uint16_t stack[16]{};
	uint8_t sp{};
	uint8_t delayTimer{};
	uint8_t soundTimer{};
	uint16_t opcode;

	std::default_random_engine randGen;

	// FIX: MSVC does not allow uint8_t here.
	// Changed from uint8_t to unsigned short.
	std::uniform_int_distribution<unsigned short> randByte;

	// The 34 Instructions
	void OP_00E0();
	void OP_00EE();
	void OP_1nnn();
	void OP_2nnn();
	void OP_3xkk();
	void OP_4xkk();
	void OP_5xy0();
	void OP_6xkk();
	void OP_7xkk();
	void OP_8xy0();
	void OP_8xy1();
	void OP_8xy2();
	void OP_8xy3();
	void OP_8xy4();
	void OP_8xy5();
	void OP_8xy6();
	void OP_8xy7();
	void OP_8xyE();
	void OP_9xy0();
	void OP_Annn();
	void OP_Bnnn();
	void OP_Cxkk();
	void OP_Dxyn();
	void OP_Ex9E();
	void OP_ExA1();
	void OP_Fx07();
	void OP_Fx0A();
	void OP_Fx15();
	void OP_Fx18();
	void OP_Fx1E();
	void OP_Fx29();
	void OP_Fx33();
	void OP_Fx55();
	void OP_Fx65();

	// Function Pointer Table
	void Table0();
	void Table8();
	void TableE();
	void TableF();
	void OP_NULL();

	typedef void (Chip8::* Chip8Func)();
	Chip8Func table[0xF + 1]{};
	Chip8Func table0[0xE + 1]{};
	Chip8Func table8[0xE + 1]{};
	Chip8Func tableE[0xE + 1]{};
	Chip8Func tableF[0x65 + 1]{};
};