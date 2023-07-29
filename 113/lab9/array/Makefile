CC = g++
CCFLAGS = -Wall -Wextra -std=c++2a -g -O3 -flto
HH = $(wildcard *.h)
SRC = $(wildcard *.cc)
OBJ = $(SRC:%.cc=%.o)

a.out: $(OBJ)
	$(CC) $(CCFLAGS) $^

%.o: %.cc $(HH)
	$(CC) $(CCFLAGS) -c $<

clean:
	rm -f a.out *.o