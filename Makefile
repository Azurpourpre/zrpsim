CC := g++
CFLAGS := -Wall -Wextra -g
LDFLAGS := 

build_main:
	$(CC) $(CFLAGS) src/main.cpp $(LDFLAGS) -o zrpsim