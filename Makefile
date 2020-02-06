
CC = g++
LDLIBS = -lncurses

objects = game.o snake.o

snake : $(objects)

$(objects) : snake.h

.PHONY: clean
clean:
	$(RM) snake *.o




