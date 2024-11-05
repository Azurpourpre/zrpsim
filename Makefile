CC=g++
CFLAGS=-Wall -Wextra -ggdb -fsanitize=address

OBJDIR=objs
FILES=$(addprefix $(OBJDIR)/, fifo.o shift.o gpio.o state_machine.o main.o)

.PHONY=build clean

build: $(FILES)
	$(CC) $(CFLAGS) $(FILES) -o zrpsim

clean:
	rm -rf objs
	rm zrpsim

$(OBJDIR)/%.o: src/*/%.cpp $(OBJDIR) 
	$(CC) -c $< $(CFLAGS) -o $@

$(OBJDIR)/%.o: src/%.cpp $(OBJDIR) 
	$(CC) -c $< $(CFLAGS) -o $@

$(OBJDIR):
	mkdir $(OBJDIR)