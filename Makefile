CC=g++
CFLAGS=-Wall -Wextra -ggdb -fsanitize=address -std=c++20

OBJDIR=objs
OBJECTS=fifo.o shift.o gpio.o state_machine.o main.o
FILES=$(addprefix $(OBJDIR)/, $(OBJECTS))

.PHONY=build clean

build: $(FILES)
	$(CC) $(CFLAGS) $(FILES) -o zrpsim

clean:
	rm -rf objs
	rm zrpsim

$(OBJDIR)/%.o: src/*/%.cpp $(OBJDIR) 
	$(CC) -c $< $(CFLAGS) -o $@

$(OBJDIR)/main.o: src/main.cpp $(OBJDIR) 
	$(CC) -c $< $(CFLAGS) -o $@

$(OBJDIR):
	mkdir $(OBJDIR)