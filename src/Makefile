CFLAGS=-I/usr/include/libxml2 -lxml2 -lm -g
sources := $(wildcard *.java)
classes := $(sources:.java=.class)

build: $(sources)
	javac -encoding UTF-8 -cp commons-math3-3.2.jar:. $(sources)

%.class: %.java
	javac -encoding UTF-8 -cp commons-math3-3.2.jar:. $<

simulator: *.c *.h
	gcc *.c $(CFLAGS) -o simulator

run: simulator
	./simulator

debug: simulator
	gdb simulator

stats: simulator Stats.class
	./roda.bash

.PHONY:	stats debug run build
