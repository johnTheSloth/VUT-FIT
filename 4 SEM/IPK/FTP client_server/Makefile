GCC= g++
CFLAGS= -std=c++11 -Wall -Wextra

all: client server

client:
	$(GCC) $(CFLAGS) ftrest.cpp -o ftrest

server:
	$(GCC) $(CFLAGS) ftrestd.cpp -o ftrestd

clean:
	rm *o ftrest ftrestd
