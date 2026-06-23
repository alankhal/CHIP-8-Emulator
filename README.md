# CHIP-8 Emulator

A cycle-driven CHIP-8 interpreter written in modern C++ with an SDL2 front end. CHIP-8 is a tiny virtual machine from the mid-1970s — 4&nbsp;KB of memory, 16 registers, a 64×32 monochrome display, and 35 opcodes — that was designed to make it easy to write small games on early microcomputers. I built this to learn how a real machine fetches, decodes, and executes instructions from the ground up, and then I wrote my own game in CHIP-8 assembly to prove the whole thing works end to end.

> _Add a screenshot or GIF of Pong here once you've captured one — `docs/pong.gif` is a good spot._

## Features

- All 35 opcodes implemented and dispatched through nested function-pointer tables (an O(1) decode rather than a giant `switch`).
- A clean split between the **core** (`Chip8`) and the **platform layer** (`Platform`), so the interpreter knows nothing about SDL and could be re-targeted to another renderer without touching the CPU.
- Fontset, sprite drawing with XOR collision detection, the full keypad, and the delay/sound timers.
- A custom game — **Pong** — that I wrote myself in CHIP-8 assembly and assembled to a ROM the emulator can run.

## How it works

The heart of the emulator is the fetch–decode–execute loop in `Chip8::Cycle()`:

1. **Fetch** the 16-bit opcode by combining two consecutive bytes (`memory[pc] << 8 | memory[pc + 1]`).
2. **Decode** it by using the top nibble to index a master function-pointer table. Opcode families that share a leading nibble (`0x0`, `0x8`, `0xE`, `0xF`) branch into a secondary table keyed on the low bits.
3. **Execute** the resolved instruction, which mutates registers, memory, the display buffer, or the program counter.

Programs are loaded at address `0x200`, the built-in font lives at `0x50`, and the lower 512 bytes are the historical interpreter region.

## Getting started

You need a C++17 compiler and **SDL2**.

### Linux

```bash
# install SDL2 (Debian/Ubuntu)
sudo apt install libsdl2-dev

# build
g++ -std=c++17 src/Main.cpp src/Chip8.cpp src/Platform.cpp \
    -o chip8 $(sdl2-config --cflags --libs)
```

### Windows (Visual Studio)

1. Download the SDL2 **development** libraries from [libsdl.org](https://libsdl.org).
2. Add the SDL2 `include` folder to **C/C++ → Additional Include Directories** and the `lib\x64` folder to **Linker → Additional Library Directories**.
3. Link against `SDL2.lib` and `SDL2main.lib`, and set the subsystem to **Console**.
4. Copy `SDL2.dll` next to the built executable.

### Running

```
chip8 <Scale> <Delay> <ROM>
```

- `Scale` — how many screen pixels each CHIP-8 pixel becomes (try `10`).
- `Delay` — milliseconds between cycles; lower is faster. Different games feel right at different speeds.
- `ROM` — path to a `.ch8` file.

```bash
chip8 10 2 roms/pong.ch8
```

## Controls

CHIP-8 has a 16-key hex keypad. I map it to the left side of a QWERTY keyboard:

```
 Keyboard          CHIP-8 keypad
 1 2 3 4            1 2 3 C
 Q W E R     -->    4 5 6 D
 A S D F            7 8 9 E
 Z X C V            A 0 B F
```

`Esc` quits.

## What made this hard

A few parts taught me the most:

- **Opcode decoding.** Each instruction packs its operands into different nibbles of a 16-bit word, so almost every opcode starts with a small ritual of masking and shifting to pull out `x`, `y`, the address, or the immediate byte. Getting the masks exactly right — and routing 35 opcodes through nested dispatch tables instead of one enormous switch — was the first real test.
- **The draw instruction (`Dxyn`).** Sprites are XOR-ed onto the framebuffer, and the flag register `VF` has to be set whenever an on-pixel gets turned off. Reasoning about that collision flag, and about what should happen at the edges of the screen, took the most care of any single instruction.
- **Timing.** The spec never says how fast a CHIP-8 runs, so the instruction rate and the 60&nbsp;Hz countdown timers have to be tuned and, ideally, decoupled from each other. Picking sensible defaults that make real games playable was more subtle than I expected.
- **The "quirks."** Several opcodes behave differently across the original COSMAC VIP, the later SUPER-CHIP, and modern interpreters (for example, whether `Fx55`/`Fx65` advance the index register, and which register a shift operates on). I had to pick a consistent set and document it.
- **A toolchain gotcha.** MSVC refuses to instantiate `std::uniform_int_distribution<uint8_t>`, so the random-byte generator uses `unsigned short` and casts down — a reminder that "portable" C++ still has sharp platform edges.

## The game: Pong

To prove the emulator end to end, I didn't just download a ROM — I wrote my own. `game/pong.8o` is a single-player **Pong** written in CHIP-8 assembly: you control the left paddle, a small AI drives the right one, and the ball bounces off the walls and paddles with XOR-based collision. The assembled ROM lives at `roms/pong.ch8`.

**Controls:** `W` = up, `S` = down.

### Building the ROM

The source is written for **[Octo](https://johnearnest.github.io/Octo/)**, a CHIP-8 assembler and IDE. There are two ways to turn `pong.8o` into `pong.ch8`:

**Option A — browser (easiest):**
1. Open the [Octo web IDE](https://johnearnest.github.io/Octo/).
2. Paste in the contents of `game/pong.8o`.
3. Press **Run** to test it, then use **Binary → save** (or the export menu) to download `pong.ch8`.

**Option B — command line:**
```bash
git clone https://github.com/JohnEarnest/c-octo
cd c-octo && make            # builds octo-cli
./octo-cli ../game/pong.8o ../roms/pong.ch8
```

Then run it on this emulator:
```bash
chip8 10 2 roms/pong.ch8
```

## Known limitations & roadmap

These are the things I'd tackle next — they're also the most interesting remaining problems:

- **Decouple the render/timer loop from the CPU.** Right now a frame is presented after every instruction. Running a fixed batch of instructions per 60&nbsp;Hz frame, decrementing the timers once per frame, and presenting a single finished frame would fix sprite flicker and make timing spec-accurate.
- **Clip sprites at the screen edges** in `Dxyn` so a sprite drawn near the right or bottom edge can't index past the framebuffer.
- **Add sound.** The sound timer is tracked but never produces the buzzer tone; an SDL audio callback would close the gap.
- **Add a score display to Pong** using the built-in font (`Fx29` + `Dxyn`).
- **Make CHIP-8 quirks configurable** so the emulator can match COSMAC VIP, SUPER-CHIP, and modern behaviour per ROM.

## Acknowledgements

- Austin Morlan's CHIP-8 emulator write-up, which the architecture here is based on.
- Cowgod's CHIP-8 Technical Reference and Matthew Mikolay's _Mastering CHIP-8_ for the instruction set.
- John Earnest's [Octo](https://github.com/JohnEarnest/Octo) for assembling the game.

## License

MIT
