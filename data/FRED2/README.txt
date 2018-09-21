acey.bin
========
Created by Paul Robson who wrote this based on notes from J. Weisbecker.

Calc II.bin
===========
From: The Sarnoff Collection at TCNJ, S.572.57

I expect there is a load fault in this file. In general it looks like 1801 code but on address 547 there is a C7 instruction which is not valid on the 1801.

Computer Bowling.bin
====================
From: The Sarnoff Collection at TCNJ, S.572.36, S.572.42

Press 0 to start Computer Bowling game, press 0 again to start the game. D, E or F will throw the bowling ball. D=down, E=straight and F=up
Press C to start screen test program, before starting the test program it will start the tape not sure why. The screen test will wait for a byte and show it on the screen starting top left position (700)
Press any other key start the build in monitor. The monitor will wait for 3 digit address and then 2 digit bytes which will be stored on the displayed memory location after which the address will be increased by 1.

Curses Foiled Again.bin
=======================
From: The Sarnoff Collection at TCNJ, S.572.42

Key 6: up, B: down, D: attack, 8: back
Curses Foiled Again is written using the FPL language below.

deduce.bin
==========
Created by Paul Robson who wrote this based on notes from J. Weisbecker.

FEL-1.bin
=========
FEL binary created by Paul Robson who reproduced it with courtesy of the Hagley Museum and Library from
documents on "FRED" developments at RCA, dated 1974-75; "Fred Folder 1", Acc. 2464, Box 919 described as
the "Billie Joe Call" Collection, accession no. 2464.54.  Hagley Museum & Library, Wilmington, DE 19807

FEL-1 Example [AUD_2464_09_B41_ID27_01_2 ].bin
==============================================
From: Hagley Museum and Library: Sarnoff/RCA Collection -Joseph A. Weisbecker's archived manuscripts and materials -Accession 2464 -Box Number AVD B41

Simple FEL-1 example program

FPL-2.bin
=========
From: The Sarnoff Collection at TCNJ, S.572.87

One of the RCA pseudo languages, one game using this language is Curses Foiled Again.

FPL-4 Race Example [AUD_2464_09_B41_ID26_02_2 ].bin
===================================================
From: Hagley Museum and Library: Sarnoff/RCA Collection -Joseph A. Weisbecker's archived manuscripts and materials -Accession 2464 -Box Number AVD B41

Simple FPL-4 example program

FPL-4 Random Example [AUD_2464_09_B41_ID26_01_2].bin
====================================================
From: Hagley Museum and Library: Sarnoff/RCA Collection -Joseph A. Weisbecker's archived manuscripts and materials -Accession 2464 -Box Number AVD B41

Simple FPL-4 example program

Fred Demo.bin
=============
From: The Sarnoff Collection at TCNJ, S.572.53

FRED demo program running demo of the FRED computer combined with the voice on a tape (Fred Demo.wav)

Following notes about the text by by Herb Johnson:
voice is Joe Weisbecker's Wife, Jean Ann Weisbecker. Daughter
Joyce Weisbecker confirmed this while visiting The Sarnoff Collection.
- notes and transcription by Herb Johnson Mar 2 2018

This is a low cost home and school computer called "FRED". It can
be programmed for fun, and games, or for simulating educational
exercises. Motors lights switches photocells and other devices
can be plugged into FRED, if they're needed.

Here's are some things FRED can do. It can ask questions
and wait for you to press the right answer button. For example,
"what is 2 plus 4?". beep

FRED can also show numbers on television. beep

How about an electronic organ? Press zero when you're
finished playing around. beep

Here's a boat. Push matching number buttons to sink the boat
you have 20 seconds press A to play. beep

Here's a simple game. You have ten seconds to catch the spot
in the box. push 2 4 6 or 8 to move the box. push A to start. beep

Want to write a program? push 2 4 6 and 8 to program the
spot. Push A and the spot will repeat the motions you
just programmed. beep

This has been a short demonstration of FRED [???] It can start or
stop a cassette, show numbers, words and pictures on television.
make sound and find which button you pushed.
It can be programmed to do any number of different things.
Ask to see a demonstration of other FRED programs. beep

GPL-A (2K RAM).bin
==================
From: The Sarnoff Collection at TCNJ, S.572.87, S.572.71

Two variants of the GPL-A pseudo language have been found, one for 2K RAM and the other running in a 2K ROM. Both variants are very similar. 
The RAM version is using screen location 0700-07FF and V0-F are stored on 0680-068F whereas the ROM version needs more memory. Interrupt routines are different, I think they have slightly different code for the timers. Command syntax is more or less the same for both with differences in:

SUB Vz, Vy-Vx on GPL I and SUB Vz, Vx-Vy on GPL II
SE Vx, [6aa] on GPL I and SNE Vx, kk on GPL II
SNE Vx, [6aa] on GPL I and SNE Vx, [8aa] on GPL II
LD [6aa], Vx on GPL I and LD [8aa], Vx on GPL II

GPL-A (2K ROM).bin
==================
From: The Sarnoff Collection at TCNJ, S.572.87, S.572.71

GPL-A (2K ROM) is used in the games 'Tag-Bowling' and 'Katch' running on the FRED 2 and RCA Video Coin Arcade machine. Katch looks like an early version of Tag-Bowling.

jackpot.bin
===========
Created by Paul Robson who wrote this based on notes from J. Weisbecker.

life.bin
========
Created by Paul Robson who wrote this based on notes from J. Weisbecker.

match.bin
=========
Created by Paul Robson who wrote this based on notes from J. Weisbecker.

spacewar.bin
============
Created by Paul Robson who wrote this based on notes from J. Weisbecker.

speedway.bin
============
Created by Paul Robson who wrote this based on notes from J. Weisbecker.

Tag-bowling.bin
===============
This is a FRED2 game even if the same exact copy runs on the RCA Video Coin Arcade emulator.

Tape Software [AUD_2464_09_B41_ID33_02].bin
===========================================
From: Hagley Museum and Library: Sarnoff/RCA Collection -Joseph A. Weisbecker's archived manuscripts and materials -Accession 2464 -Box Number AVD B41

Save/load software, to load a program from tape press 0, code will be loaded at address 0x100. To save memory to tape press C (memory 0x100-0x7FF will be saved to tape).

XMAS Music & Show [AUD_2464_09_B41_ID30].bin
============================================
From: Hagley Museum and Library: Sarnoff/RCA Collection -Joseph A. Weisbecker's archived manuscripts and materials -Accession 2464 -Box Number AVD B41

Christmas demo
