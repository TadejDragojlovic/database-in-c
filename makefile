FILENAME = database
EXENAME = db

all: compile run clean

compile: 
	gcc $(FILENAME).c -o $(EXENAME)

run:
	./$(EXENAME)

clean:
	rm -r $(EXENAME)