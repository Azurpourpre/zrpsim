CC=g++
CFLAGS=-Wall -Wextra -ggdb -fsanitize=address
LDFLAGS=

build_main:
	$(CC) $(CFLAGS) src/main.cpp $(LDFLAGS) -o zrpsim
