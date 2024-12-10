CC = gcc
CFLAGS = -Wall -O2

all: calculator

calculator: calculator.c
	$(CC) $(CFLAGS) calculator.c -o calculator

clean:
	rm -f calculator
