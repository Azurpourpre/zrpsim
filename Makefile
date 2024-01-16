CC := g++
CFLAGS := -Wall -Wextra -g -fsanitize=address
LDFLAGS := 

build_main:
	$(CC) $(CFLAGS) src/main.cpp $(LDFLAGS) -o zrpsim