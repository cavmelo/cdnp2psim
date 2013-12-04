CFLAGS=-I/usr/include/libxml2 -lxml2 -lm -g

simulator: *.c *.h
	gcc *.c $(CFLAGS) -o simulator

run: simulator
	./simulator

debug: simulator
	gdb simulator
