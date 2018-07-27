The Sarnoff Collection at TCNJ, S.572.96, S.572.94, S.572.75, S.572.66, S.572.61, , S.572.38, S.572.26

1K Memory Test.bin
==================

21 I.bin
========

Use cards as follows:
0: 0xBD - new game & player one card,
1: 0xCC - Give player one card
2: 0x78 - Give dealer one card 
4: 0x73 - Show hand player
5: 0xB1 - Show hand dealer

21-II.bin
=========

1=Deal new hand
2=Give player one card
4=Give dealer one card
8=Player sticks, dealer can get more cards now.
New hand flashes number of hands won by P&D so far (up to 10). Simulates actual deck of cards. 5 player cards under 21 count for 2 player. "SHUFFLE" indicates new deck.

Acey Duecy.bin
==============

Two playing card values appear in the upper left corner. $25 appears in the upper right corner. This is the money you have. The flashing question marks mean that the computer is waiting for you to place your bet. You will bet that the value of the next card dealt by the computer lies between the two card values shown (not equal to either). Bet by pressing any keyboard positions (2 = 2$, 8 = $8, A = $10, F = $15). You can bet a maximum of $15 (press F) and a minimum of $1 (press 1). You can' bet more than you have. You have to get $100 to break the bank. New game = reset/run.

Add Drill.bin
=============

This SW needs spoken text and tones recorded on original tapes

Animate demo.bin
================

Demo which loads data from tape and displays that data on the screen. Data is included in 'Animate demo data.bin' file but is not loadable as such in Emma 02.

Binary.bin
==========

On startup press 3, 4, 5 or 6 for the number of bits. Then the computer will show a binary figure, convert this to decimal and type the answer. If correct it gets counted on the 'Y' line if wrong it is counted on the 'N' line. If wrong you will need to try again.

Bowling.bin
===========
 
Two players, X & O. 5 Frames, 2 balls/frame. Enter any number to roll ball. F1, F2 etc. indicates frame number. X1, X2, O1, O2 indicates player and ball number. New game = reset/run.

Calculate.bin
=============

Use 0 to 9 for numbers, A=sign, B='.' and D=CLR
Shift: 1=(, 2=X, 3=), 4=-, 5=Result, 6=/, 7=<-, 8=->, 0=+

Cardtran.bin
============

CARDTRAN Interpreter, see Cardtran.readme or Cardtran.asm for more information

Cartoon.bin
===========

Clue.bin
========

As described in manual: http://www.retrotechnology.com/memship/Sarnoff_FRED_manual_Weisbecker_Robbi.pdf

Deduce.bin
==========

The computer will choose a 4 digit random number which you must guess within 15 turns to win. No two digits in the number are the same. Enter your first guess by pressing 4 keyboard digits. The computer then provides some clues. 0 2 means 2 of the digits you entered match two digits of the unknown number but are not in the correct position. 0 means none of your digits are in the correct position. 2 1 means you have 2 digits right and in the proper position and 1 digit right but in the wrong position. Can you develop a strategy for always guessing the number within 15 tries? New game = reset/run.

Deduce-leds.bin
===============

As described in manual: http://www.retrotechnology.com/memship/Sarnoff_FRED_manual_Weisbecker_Robbi.pdf

Demo.bin
========

Demo using spoken text and tones recorded on original tapes

Digit Display.bin
=================

Type in digit 0 to F and a representation will be shown on the screen

Display.bin
===========

As described in manual: http://www.retrotechnology.com/memship/Sarnoff_FRED_manual_Weisbecker_Robbi.pdf

Draw.bin
========

Drawing program, screen is divided in 4 x 4 array. To draw first press array position as in keyboard layout, then press symbol. Key 0 to D will show a symbol, E will erase the cell and F will erase the screen.

Eng. Apt. Test.bin
==================

This SW needs spoken text and tones recorded on original tapes

Erase.bin
=========

2 players take turns. Enter any number to erase marks under moving line. Player who erases last vertical mark wins. Trying to erase when no mark is under line causes player to lose. New game = reset/run.

ESP I.bin
=========

Not sure what this game is about, maybe guessing a number?

ESP II.bin
==========

Not sure what this game is about, maybe guessing a number?
Also named 'Logi Hex' on one of the tape labels.

Flip I.bin
==========

Start game with card '0' (twice). The game uses two players, white and black square. First player to get 8 squares of his type wins. Centre square indicates which player gets the next turn. Use card 1 (0x6d), 2 (0x97) or 3 (0x9e) to flip squares. If no player wins by turn 50 the player with the most squares wins.

Flip II.bin
===========

Start game with card '0' (twice). The game uses two players, white and black square. First player to get 8 squares of his type wins. Centre square indicates which player gets the next turn. Use keys 1, 2 or 3 to flip squares. If no player wins by turn 50 the player with the most squares wins.

Hexapawn I.bin
==============

Not sure what this game is about, possibly just a work in progress?

Hexapawn II.bin
===============

Not sure what this game is about, possibly just a work in progress?

Hi-Lo.bin
=========

Guess the number between 0 and 100, computer will state if it is 'H' i.e. higher or 'L' lower.

Jackpot.bin
===========

$10 should appear on the screen. Pushing any key will cause $1 to be subtracted from your initial $10. Three changing symbols will appear. Pressing any key will freeze the first symbol. Press two more times to freeze the remaining symbols. If you freeze three different symbols you win #2. If you freeze three identical symbols you win $5 or $10. After the computer shows whether you won or lost, press any key to play again. The computer shows how much money you have. If you go broke, you lose. If you accumulate $50 you break the bank. New game = reset/run.

Life.bin
========

A flashing dot appears in the upper left corner of the TV screen. Press key 1 (left), 2 (up), 3 (right) and 5 (down) to move the dot within a 32x32 position field. Press 4 to permanently write a spot on the screen in the position of the flashing dot and press 6 to erase the permanent spot from the screen. Pictures can be created in this manner. Press '0' to start cell generation, new generations are shown at 30 second intervals.

Luckypath.bin
=============

Press two byte codes, every code will make a random path, when the path connects from left to right you win.

Match I.bin
===========

This game is for two players (1 and 2). The object is to find matching pairs of symbols. Press card '0' to start the game (or FF in BYTE mode). A 4 x 4 array of symbols will flash on the screen. The 16 keyboard positions represent symbol positions on the screen. Player 1 presses two keyboard positions trying to find two matching symbols. If he finds two that match, they are replaced by 1 on the screen. He then gets another turn. If he doesn't find two matching symbols it is player 2's turn next. Turns alternate in this fashion until all symbols have been matched. Players always get extra turns for finding matching symbols. Count 1's and 2's showing to determine winner of game. New game = reset/run.

Match II.bin
============

Same game as Match.bin with different cards. To start press '0' instead of 'FF'

Memory Test.bin
===============

Press a digit between 00 and 80 and after that a second byte. The second byte will be stored in memory 01xx where xx is the first byte. Also a 'binary' representation is shown on the screen. The screen has 128 positions reserved for the 128 memory locations.

Merry Christmas.bin
===================

Merry Christmas moving text demo

Minikrieg.bin
=============

Keys:
6: Up
B: Down
8: Left
D: right
Move spot to symbols, press 'A' to lock a symbol and move it with the keys, some directions don't work. 'C' drops the symbol and you can move to another.

What the aim of the game is I don't know, maybe this is some strategic game?

Nimnet.bin
==========

Similar to Lukcypath, uses cards with hex codes A1/A2, B1-B4, C1-C5, D1-D5, E1-E4 and F1/F2. I think the cards were not marked and with luck you had to create a path from the left 'dot' to the right 'dot' using random cards.

Pattern Puzzle.bin
==================

Pattern shown on screen is a combination of 2 out of 16 patterns. The patterns are shown in the 'Pattern Puzzle.pdf' file. If you enter the two correct pattern numbers you will get a new pattern.

Prog. Apt. Test.bin
===================

This SW needs spoken text and tones recorded on original tapes

Seasons Greetings.bin
=====================

Another Christmas demo

Slide Puzzle I.bin
==================

Start game with card 0 (value 0) followed by card 1 (0x31), 2 (0x33) or 3 (0x35). Card 1, 2 or 3 will select the game type. Move squares with card:
6 (0x6): up
12 (0x1): left
13 (0xff): right
18 (0xfa): down

For card 2/3 try to get a pattern of squares symmetrical about the centre row in a minimum number of turns (10 for card 2). For card 1 you must swap the 'full' and 'empty' squares and keep the rest as they are at the start in no more than 18 turns.

Slide Puzzle II.bin
===================

Start game with key 0 followed by key A, B or C. Key A, B or C will select the game type. Move squares with key:
2: up
4: left
6: right
8: down

For B/C try to get a pattern of squares symmetrical about the centre row in a minimum number of turns (10 for B). For A you must swap the 'full' and 'empty' squares and keep the rest as they are at the start in no more than 18 turns.

Space War.bin
=============

Your ship is on the left. Move it up and down by pressing key 1 and 3. Fire at computer ship by pressing 2. If you hit the computer ship 16 times first you win. New game = reset/run.

Spook.bin
=========

Spooky demo

Spot Speedway I.bin
===================

Press '1' or '2' (different layout) to start spot & clock. Use 6 (up), 8 (left) , B (down) and D (right) to change direction.
C=Decelerate, A=Accelerate mode. Spot starts in 'A' mode. Get from start to end in minimum clock time without hitting walls.

Spot Speedway II.bin
====================

Same as 1 with a different race track.

Target.bin
==========

Press any key when a symbol is shown, if on time the symbol will explode.

Tic-Tac-Toe.bin
===============

Computer is 'O', player is 'X'. Enter the number of the square you want to mark. New game = reset/run.
Field:
 2 1 0
 5 4 3
 8 7 6

TV Sound #1.bin
===============

Utility I.bin
=============

Utility II.bin
==============

Utility II.bin
==============

Write 0-1.bin
=============

Write CD I.bin
==============

As described in manual: http://www.retrotechnology.com/memship/Sarnoff_FRED_manual_Weisbecker_Robbi.pdf

Write CD II.bin
===============
