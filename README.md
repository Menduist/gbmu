# Gameboy emulator

A C Gameboy Emulator. Not all opcodes are handled, so it won't work on every game.

The interesting part of this project is the way opcodes are handled. Since they are 256 opcodes on a Gameboy, the goal is to have as little code per opcode as possible. To do so, preprocessing is heavily used.

The `opcode` file contains the description of each opcode and their functionnality (only 1 line per opcode). It's then included at the end of `instructions.c` two times : Once to create all the functions automatically, and a second time to create an opcode array.

This is the most conveniant way to do this, and I've use this kind of trick multiple times in C to avoid repetition.
