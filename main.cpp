#include <CppWeb.h>
#include <iostream>
#include <set>
#include <signal.h>

using std::set;

class NoopChat : public WebListener {
	private:
		set<int> fds;
		CppWeb cppWeb;

	public:
		NoopChat()
			: cppWeb(*this){
		}
	
		~NoopChat() {
			cppWeb.stop();
		}

		void
		start() {
			cppWeb.start(8000);
		}

		void
		onData(int fd, unsigned char *data, unsigned int size) {
			if( data == NULL || size == 0 || fds.count(fd) == 0 ) {
				return;
			}

			for( int toFd : fds ) {
				cppWeb.send(toFd, data, size);
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

volatile bool isRunning = true;

void sig_handler(int signo)
{
	if( signo == SIGINT ) {
		std::cout << "Shutting down ..." << std::endl;
		isRunning = false;
	}
}

int
main( int argv, char **argc ) {
	
	if( signal(SIGINT, sig_handler) == SIG_ERR ) {
		std::cout << "Could not register SIGINT handler" << std::endl;
		std::cout.flush();
		return -1;
	}
	
	NoopChat noopChat;
	noopChat.start();

	while( isRunning ) {
		usleep(1000000);
	}
	return 0;
}
