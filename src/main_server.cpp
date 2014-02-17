#include "multi_thread_server.h"
#include "select_thread_server.h"
#include "epoll_server.h"
void* printThread(void * args);
int main(int argc, char **argv)
{
	int port;
	int serverType = 0;
	switch(argc)
	{
		case 1:
			port = TCP_PORT;
			break;
		case 2:
			port = atoi(argv[1]);
			break;
		default:
			fprintf(stderr, "Usage: %s [servertype] [port]\n", argv[0]);
			exit(1);
	}
	
	switch(serverType){
		case 1:
			MultiThreadServer* server = MultiThreadServer::Instance();
			break;
		case 2:
			SelectServer* server = SelectServer::Instance();
			break;
		case 3:
		default:
			EpollServer* server = EpollServer::Instance();
			break;
	}
	char filename[] = "test/tests.txt";
	ClientData::Instance()->setFile(filename);

	pthread_t tid;
	pthread_create(&tid, NULL, printThread, (void*)NULL);

	server->set_port(port);
	server->run();
	pthread_kill(tid, SIGTERM);
	return 0;
}

void* printThread(void * args){
	
	while(1){
		sleep(1000);
		ClientData::Instance()->print();
	}
	return (void*)0;
}


