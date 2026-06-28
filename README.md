<img width="1278" height="866" alt="go-chip8-flickering" src="https://github.com/user-attachments/assets/cb7af65b-2e6c-4741-b2d6-795182ed9ba4" />
# CHIP-8 Emulator

A cycle-driven CHIP-8 interpreter written in modern C++ with an SDL2 front end, plus **Pong**, a game I wrote from scratch in CHIP-8 assembly to prove the emulator works end to end.

CHIP-8 is a tiny virtual machine from the mid-1970s — 4&nbsp;KB of memory, 16 registers, a 64×32 monochrome display, and 35 opcodes — designed to make it easy to write small games on early microcomputers. I built this to learn, from the ground up, how a real machine fetches, decodes, and executes instructions.

<img width="1278" height="866" alt="go-chip8-flickering" src="https://github.com/user-attachments/assets/cb7af65b-2e6c-4741-b2d6-795182ed9ba4" />

## Project structure

```
CHIP-8-Emulator/
├── CHIP-8 Emulator.sln        # Visual Studio solution
├── src/                       # emulator source + project files
│   ├── CHIP-8 Emulator.vcxproj
│   ├── Chip8.cpp / Chip8.hpp  # the CPU core: memory, registers, all 35 opcodes
│   ├── Platform.cpp / .hpp    # SDL2 window, rendering, and keyboard input
│   └── Main.cpp               # entry point and the 60 Hz run loop
├── game/
│   └── pong.8o                # Pong, in CHIP-8 assembly (Octo source)
├── roms/
│   └── pong.ch8               # the assembled, ready-to-run ROM
└── docs/                      # screenshots / media
```

## Features

- All 35 opcodes, dispatched through nested function-pointer tables (an O(1) decode rather than a giant `switch`).
- A clean split between the **core** (`Chip8`) and the **platform layer** (`Platform`), so the interpreter knows nothing about SDL and could be re-targeted to another renderer without touching the CPU.
- Sprite drawing with XOR collision detection that **clips safely at the screen edges**.
- 60 Hz delay/sound timers that are **decoupled from CPU speed**, with a render loop that presents one finished frame at a time (no flicker).
- A custom game — **Pong** — written in CHIP-8 assembly and assembled to a ROM.

## How it works

The heart of the emulator is the fetch–decode–execute cycle in `Chip8::Cycle()`:

1. **Fetch** the 16-bit opcode by combining two consecutive bytes (`memory[pc] << 8 | memory[pc + 1]`).
2. **Decode** it by using the top nibble to index a master function-pointer table. Opcode families that share a leading nibble (`0x0`, `0x8`, `0xE`, `0xF`) branch into a secondary table keyed on the low bits.
3. **Execute** the resolved instruction, which mutates registers, memory, the display buffer, or the program counter.

`Main.cpp` runs a batch of these cycles per 60 Hz frame, ticks the timers once, and presents a single frame. Programs are loaded at address `0x200`, the built-in font lives at `0x50`, and the lower 512 bytes are the historical interpreter region.

## Building and running (Windows + Visual Studio)

**Requirements:** Visual Studio 2022 (toolset v143) and **SDL2** (developed against SDL2 2.30.12).

This project is configured to build for **x64** — the SDL2 include/library paths are only set on the x64 configuration, so make sure the platform dropdown says **x64**, not x86/Win32.

### One-time SDL2 setup

The project expects SDL2 at `C:\SDL2-2.30.12`. Either:

- download the SDL2 **VC development** package from [libsdl.org](https://github.com/libsdl-org/SDL/releases) and extract it to `C:\SDL2-2.30.12`, **or**
- point the project at your own location: **Project → Properties → C/C++ → Additional Include Directories** and **Linker → Additional Library Directories**.

Then copy `SDL2.dll` (from the SDL2 `lib\x64` folder) next to the built executable. With this project's default settings the `.exe` is written to a solution-level folder, **`x64\Debug\`** at the repo root (or `x64\Release\`), so put `SDL2.dll` there. If you're unsure, the exact path is printed in the **Build → Output** window after a build. Without the DLL you'll get a "SDL2.dll was not found" error at launch.

### Run from Visual Studio

1. Open `CHIP-8 Emulator.sln` and set the configuration to **Debug / x64**.
2. Build with **Ctrl+Shift+B**.
3. The emulator takes command-line arguments, so tell Visual Studio what to pass: **Project → Properties → Configuration Properties → Debugging**, set **Configuration** to *All Configurations*, and fill in:
   - **Command Arguments:** `10 10 roms\pong.ch8`
   - **Working Directory:** `$(SolutionDir)`
4. Press **Ctrl+F5** (Start Without Debugging). A window titled **CHIP-8 Emulator** opens.

### Run from a terminal

After building, from the repo root:

```
x64\Debug\"CHIP-8 Emulator.exe" 10 10 roms\pong.ch8
```

### Arguments

```
<Scale> <CyclesPerFrame> <ROM>
```

- `Scale` — how many screen pixels each CHIP-8 pixel becomes (try `10`).
- `CyclesPerFrame` — instructions executed per 60 Hz frame; **higher = faster**. `10` is a good start for Pong.
- `ROM` — path to a `.ch8` file.

## Controls

CHIP-8 has a 16-key hex keypad, mapped to the left side of a QWERTY keyboard:

```
 Keyboard          CHIP-8 keypad
 1 2 3 4            1 2 3 C
 Q W E R     -->    4 5 6 D
 A S D F            7 8 9 E
 Z X C V            A 0 B F
```

`Esc` quits.

## The game: Pong

`game/pong.8o` is a single-player **Pong** I wrote in CHIP-8 assembly: you control the left paddle, a small AI drives the right one, and the ball bounces off the walls and paddles using XOR collision. The assembled ROM is `roms/pong.ch8`.

### How to play

1. Launch the emulator with the ROM (see above). Your paddle is on the left; the computer's is on the right.
2. Hold **`W`** to move up and **`S`** to move down.
3. Line your paddle up with the ball so it ricochets back — the rally continues as the ball bounces between you and the AI.
4. Keep the ball alive: don't let it slip past your paddle on the left edge.
5. If the ball gets past either paddle, it re-serves from the centre in a random direction.
6. Press **`Esc`** to quit.

**Tip:** the AI only chases the ball while it's heading toward it, so a sharp return near your own paddle can wrong-foot it. Adjust `CyclesPerFrame` to taste if it feels too fast or slow.

### Rebuilding the ROM from source

The game is written for **[Octo](https://johnearnest.github.io/Octo/)**, a CHIP-8 assembler and IDE.

**Browser (easiest):** open the [Octo web IDE](https://johnearnest.github.io/Octo/), paste in `game/pong.8o`, press **Run** to test, then export the binary as `pong.ch8`.

**Command line:**
```
git clone https://github.com/JohnEarnest/c-octo
cd c-octo && make
./octo-cli ../game/pong.8o ../roms/pong.ch8
```

## Roadmap

- Add a score display and win condition to Pong using the built-in font (`Fx29` + `Dxyn`).
- Emit the buzzer tone from the sound timer via an SDL audio callback.
- Make CHIP-8 quirks (shift behaviour, `Fx55`/`Fx65` index increment) configurable per ROM.
- Add an x86/Win32 build configuration with matching SDL2 paths.

## Acknowledgements

- Austin Morlan's CHIP-8 emulator write-up, which the architecture here is based on.
- Cowgod's CHIP-8 Technical Reference and Matthew Mikolay's _Mastering CHIP-8_ for the instruction set.
- John Earnest's [Octo](https://github.com/JohnEarnest/Octo) for assembling the game.

## License

MIT
