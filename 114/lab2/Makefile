CC = g++
CCFLAGS = -Wreturn-type -Wall -Wextra -std=c++11 -g -O3 -flto
SRC = $(wildcard *.cpp)

a.out: $(SRC)
	$(CC) $(CCFLAGS) $^

clean:
	rm -f a.out *.o
