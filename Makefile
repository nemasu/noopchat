CCOPTS=-std=c++11 -Wall -Werror -I/usr/include/libasock -I/usr/include/libcppweb

all: CCOPTS += -O2
all: noopchat strip

debug: CCOPTS += -g
debug: noopchat

noopchat: main.cpp
	g++ ${CCOPTS} main.cpp -o noopchat -lcppweb

strip:
	strip --strip-all noopchat
	strip --strip-debug noopchat

clean:
	rm -rf *.o noopchat
