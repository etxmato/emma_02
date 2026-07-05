PROJECT = rclisp

$(PROJECT).prg: $(PROJECT).asm bios.inc
	rcasm -l -v -x -h -d1802 $(PROJECT)

clean:
	-rm $(PROJECT).prg


