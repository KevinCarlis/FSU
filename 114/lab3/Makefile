CC = g++
CCFLAGS = -pthread -Wall -Wextra -std=c++11 -g -O3 -flto -o P3.exe
SRC = $(wildcard *.cpp)

P3.exe: $(SRC)
	$(CC) $(CCFLAGS) $^

clean:
	rm -f P3.exe *.o
