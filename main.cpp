#include <CppWeb.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <iostream>
#include <unordered_map>
#include <signal.h>

#define MAX_BUFFER_SIZE 65535

using namespace rapidjson;

using std::unordered_map;

struct UserData {
	UserData() {
		name = "Anonymous";
	}
	string name;
};

class NoopChat : public WebListener {
	private:
		static constexpr const char* KEY_NAME = "name";
		static constexpr const char* KEY_MSG  = "msg";
		
		unordered_map<int, UserData> users;
		CppWeb cppWeb;
		char recvBuffer[MAX_BUFFER_SIZE];
		int recvBufferIdx;

	public:
		NoopChat()
			: cppWeb(*this){
			recvBufferIdx = 0;
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
			if( data == NULL || size == 0 || users.count(fd) == 0 ) {
				return;
			}

			//Concat buffer
			if ( size + recvBufferIdx >= MAX_BUFFER_SIZE-1 ) {
				//Something is not right, start over
				std::cerr << "Buffer size error" << std::endl;
				recvBufferIdx = 0;
			}

			memcpy( recvBuffer + recvBufferIdx, data, size ); 
			memcpy( recvBuffer + recvBufferIdx + size, "\0", 1 ); 

			//Deserialize
			Document d;
			d.Parse(recvBuffer);

			//Handle
			if( !d.HasParseError() ) {
				if (   d.HasMember(KEY_NAME)
					&& d[KEY_NAME].IsString() ) {
				
					Value &cmd = d[KEY_NAME];
					string cmdValue = cmd.GetString();
					users[fd].name = cmdValue;
					
				}
				
				if (   d.HasMember(KEY_MSG)
					&& d[KEY_MSG].IsString() ) {
					
					Value &cmd = d[KEY_MSG];
					string cmdValue = cmd.GetString();
					for( auto &e : users ) {
						string msg = e.second.name + ": " + cmdValue;
						cppWeb.send(e.first, (unsigned char *) msg.c_str(), msg.length());
					}

				}
			}
			
		}

		void
		onConnect( int fd ) {
			users[fd];
		}

		void
		onClose( int fd ) {
			users.erase(fd);
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
