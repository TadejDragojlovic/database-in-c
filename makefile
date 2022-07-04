CC = gcc
CL = clang
CFLAGS = -I$(IDIR) -g

IDIR = ./include/
SRCDIR = ./src/
EXENAME = db

SOURCES = $(SRCDIR)*.c

all: build

clang:
	$(CL) $(SOURCES) $(CFLAGS) -o $(EXENAME)

build: 
	$(CC) $(SOURCES) $(CFLAGS) -o $(EXENAME)

debug:
	$(CC) $(SOURCES) -DDEBUG_NODE_INFO $(CFLAGS) -o $(EXENAME)

run:
	./$(EXENAME)

clean:
	rm -r $(EXENAME)
