FILENAME1 = database
FILENAME2 = buffer
EXENAME = db

all: compile run clean

compile: 
	gcc $(FILENAME1).c $(FILENAME2).c -o $(EXENAME)

run:
	./$(EXENAME)

clean:
	rm -r $(EXENAME)