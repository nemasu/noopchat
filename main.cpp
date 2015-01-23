#include <CppWeb.h> 
#include <set>

using std::set;

class NoopChat : public WebListener {
	private:
		set<int> fds;
		CppWeb *cppWeb;

	public:
		NoopChat() {
			cppWeb = new CppWeb(this);
		}
	
		~NoopChat() {
			delete cppWeb;
		}

		void
		start() {
			cppWeb->start(8000);
		}

		void
		onData(int fd, unsigned char *data, unsigned int size) {
			if( data == NULL || fds.count(fd) == 0 ) {
				return;
			}

			for( int toFd : fds ) {
				cppWeb->send(toFd, data, size);
			}
		}

		void
		onConnect( int fd ) {
			fds.insert(fd);
		}

		void
		onClose( int fd ) {
			fds.erase(fd);
		}

};

int
main( int argv, char **argc ) {
	NoopChat noopChat;
	noopChat.start();

	while(1) {
		usleep(1000000);
	}

	return 0;
}
