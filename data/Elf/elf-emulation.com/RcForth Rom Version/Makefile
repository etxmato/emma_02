PROJECT = forth
$(PROJECT).rom: $(PROJECT).asm
	rcasm -l -v -x -h -d1802 $(PROJECT)

clean:
	-rm $(PROJECT).prg

