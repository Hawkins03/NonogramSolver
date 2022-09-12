CC = gcc
CFLAGS = -O3 -Wall -Werror
DEPS = io.h solver.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

solver: solver.o io.o
	$(CC) -o NonogramSolver solver.o io.o $(CFLAGS)

io-debug: io.o
	$(CC) -o io io.o $(CFLAGS)

.phony: clean
clean:
	rm *.o NonogramSolver a.out io solver
