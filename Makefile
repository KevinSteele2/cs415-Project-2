CC=gcc
EXEC=part2 cpubound iobound
FLAGS=-g -Wall -o

all: $(EXEC)

part2: part2.o string_parser.o
	$(CC) $(FLAGS) $@ $^

cpubound: cpubound.o 
	$(CC) $(FLAGS) $@ $^

iobound: iobound.o
	$(CC) $(FLAGS) $@ $^    

# %< grabs left dependency

%.o: %.c 
	$(CC) -c $<    

clean:
	rm -f *.o $(EXEC)

# More about makefiles on canvas
