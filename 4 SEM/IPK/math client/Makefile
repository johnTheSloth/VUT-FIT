GCC= g++
CFLAGS= -std=c++11 -Wall -Wextra

all: client

client:
	$(GCC) $(CFLAGS) ipk-client.cpp md5.cpp -o ipk-client
clean:
	rm *o ipk-client
tgz:
	tar -cvzf xgross09.tgz ipk-client.1 Makefile ipk-client.h ipk-client.cpp md5.cpp md5.h
