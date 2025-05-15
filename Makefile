CC=gcc
EXEC=part1 part2 part3 part4
FLAGS=-g -Wall -o

all: $(EXEC)

part1: part1.o string_parser.o
	$(CC) $(FLAGS) $@ $^

part2: part2.o string_parser.o
	$(CC) $(FLAGS) $@ $^

part3: part3.o string_parser.o
	$(CC) $(FLAGS) $@ $^

part4: part4.o string_parser.o
	$(CC) $(FLAGS) $@ $^

# %< grabs left dependency

%.o: %.c 
	$(CC) -c $<    

clean:
	rm -f *.o $(EXEC)

# More about makefiles on canvas
