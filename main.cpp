#include <CppWeb.h> 
#include <set>

using std::set;

set<int> fds;

CppWeb *cw;

void
onData(int fd, unsigned char *data, unsigned int size) {
	if( data == NULL ) {
		fds.erase(fd);
		return;
	} else if (fds.count(fd) == 0) {
		fds.insert(fd);
	}
	for( int toFd : fds ) {
		cw->send(toFd, data, size);
	}
}

int
main( int argv, char **argc ) {
	cw = new CppWeb(onData);
	cw->start(8000);

	while(1) {
		usleep(1000000);
	}

	delete cw;
	return 0;
}
