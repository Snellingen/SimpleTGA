CC= gcc
SRCDIR = src
NAME = SimpleTGA

#End sources definition

DEPENDENCIES = $(shell find $(SRCDIR) -name '*.c')
OBJECTS = $(DEPENDENCIES:.c=.o)

#End dependcies definition
CFLAGS = -std=c99 -Wextra -Wall #-Werror TODO ENABLE

all: release debug

release: $(OBJECTS)
	mkdir -p bin/release
	$(CC) $(CFLAGS) -O3 -o bin/release/$(NAME) $(OBJECTS)
	mkdir -p bin/debug
debug: $(OBJECTS)
	$(CC) $(CFLAGS) -g -o bin/debug/$(NAME) $(OBJECTS)


	
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@	

clean:
	rm -f *.o
	rm -fr bin/
	
remake: clean all
