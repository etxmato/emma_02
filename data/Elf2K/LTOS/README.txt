LTOS for Emma 02
================
LTOS 0.5 (with STELLAR V0061.37) on a CompactFlash card image, for the COSMAC Elf 2000 in Emma 02 V2.00.
Older versions of Emma 02 start the card but stop at "LTOS: loading the kernel ?? FAILED (read/sum)":
update Emma 02 to V2.00 (Help, About Emma 02 shows the version).

Install: copy these into your Emma 02 data folder (the one holding the Elf2K and Xml folders):

    ltos-card.ide   into  Elf2K/
    rtcdump.bin     into  Elf2K/    (replaces any you have: without it the ROM stops at POST code 74)
    ltos.xml        into  Xml/Elf2K/

Run: in Emma 02 choose COSMAC Elf 2000, set XML to ltos.xml, check IDE 0 says ltos-card.ide, press Start.
Click in the VT 100 window and press Enter. At the >>> prompt type BOOT and press Enter.

Type BOOT first: SHOW IDE before BOOT gave ?DRIVE ERROR here. If that happens, press Reset and try again.
HELP lists the commands. Keep a copy of ltos-card.ide: Emma 02 writes to it.
Music: PLAY /MUSIC/ODE plays a tune through Emma's Q sound (the kit turns it on). Emma's Elf 2000 runs at
3.58 MHz, PLAY's own speed; on an Elf2K at another clock give it: PLAY -s 1.79 ODE. HELP PLAY has the notes.

Files to and from the card: close Emma 02 first. ltos-card.ide is a raw card image with a FAT16 partition 32768 bytes
(64 sectors) in, and names are 8.3. Linux: sudo mount -o loop,offset=32768 ltos-card.ide /mnt. Mac: hdiutil attach
-imagekey diskimage-class=CRawDiskImage ltos-card.ide. Windows: a tool that mounts raw images, such as OSFMount.
Delete any hidden files the computer leaves there before you unmount it.
More: https://stellar-1802.dev

LTOS, STELLAR and Stellar C are free software under GPL-3.0-or-later: see LICENSE.txt.
