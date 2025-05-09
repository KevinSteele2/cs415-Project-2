CC=gcc
EXEC=part1 cpubound iobound
FLAGS=-g -o

all: part1 cpubound iobound

part1: part1.o 
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