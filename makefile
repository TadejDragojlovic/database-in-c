CC = gcc
CFLAGS = -I$(IDIR)

IDIR = ./include/
SRCDIR = ./src/
EXENAME = db

SOURCES = $(SRCDIR)*.c

all: build

build: 
	$(CC) $(SOURCES) $(CFLAGS) -o $(EXENAME)

debug:
	$(CC) $(SOURCES) -DDEBUG_NODE_INFO $(CFLAGS) -o $(EXENAME)

run:
	./$(EXENAME)

clean:
	rm -r $(EXENAME)
