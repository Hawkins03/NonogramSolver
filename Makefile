CC = gcc
CFLAGS = -O3 -Wall -Werror -g
DEPS = io.h solver.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

solver: solver.o io.o
	$(CC) -o NonogramSolver solver.o io.o $(CFLAGS)

debug: debug.o io.o
	$(CC) -o debug debug.o io.o $(CFLAGS)

.phony: clean
clean:
	rm *.o NonogramSolver a.out debug solver
