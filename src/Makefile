CFLAGS=-I/usr/include/libxml2 -lxml2 -lm

simulator: *.c *.h
	gcc *.c $(CFLAGS) -o simulator

run: simulator
	./simulator > output.txt
