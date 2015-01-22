CCOPTS=-std=c++11 -Wall -Werror -I/usr/include/libasock -I/usr/include/libcppweb

all: CCOPTS += -O2
all: noopchat

debug: CCOPTS += -g
debug: noopchat

noopchat: main.cpp
	g++ ${CCOPTS} main.cpp -o noopchat -lcppweb

clean:
	rm -rf *.o noopchat
