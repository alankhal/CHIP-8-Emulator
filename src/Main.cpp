#include "Chip8.hpp"
#include "Platform.hpp"
#include <chrono>
#include <iostream>
#include <string>   // FIX: Required for std::stoi on MSVC

int main(int argc, char** argv)
{
	if (argc != 4)
	{
		std::cerr << "Usage: " << argv[0] << " <Scale> <CyclesPerFrame> <ROM>\n";
		std::exit(EXIT_FAILURE);
	}

	int videoScale = std::stoi(argv[1]);
	int cyclesPerFrame = std::stoi(argv[2]);
	char const* romFilename = argv[3];

	Platform platform("CHIP-8 Emulator",
		VIDEO_WIDTH * videoScale, VIDEO_HEIGHT * videoScale,
		VIDEO_WIDTH, VIDEO_HEIGHT);

	Chip8 chip8;
	chip8.LoadROM(romFilename);

	int videoPitch = sizeof(chip8.video[0]) * VIDEO_WIDTH;

	// Run a fixed number of instructions per 60 Hz frame, then update the
	// timers and present a single finished frame. This keeps timing
	// independent of CPU speed and eliminates the flicker you get from
	// presenting the screen after every individual instruction.
	//
	//   <CyclesPerFrame> controls how many instructions run per frame, so a
	//   larger value makes games run FASTER (more work per frame).
	int instructionsPerFrame = cyclesPerFrame;
	if (instructionsPerFrame < 1)
	{
		instructionsPerFrame = 1;
	}

	const double msPerFrame = 1000.0 / 60.0;

	auto lastFrameTime = std::chrono::high_resolution_clock::now();
	bool quit = false;

	while (!quit)
	{
		quit = platform.ProcessInput(chip8.keypad);

		auto currentTime = std::chrono::high_resolution_clock::now();
		double dt = std::chrono::duration<double, std::milli>(
			currentTime - lastFrameTime).count();

		if (dt >= msPerFrame)
		{
			lastFrameTime = currentTime;

			for (int i = 0; i < instructionsPerFrame; ++i)
			{
				chip8.Cycle();
			}

			chip8.UpdateTimers();
			platform.Update(chip8.video, videoPitch);
		}
	}

	return 0;
}