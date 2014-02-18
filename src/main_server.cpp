#include "multi_thread_server.h"
#include "select_server.h"
#include "epoll_server.h"
void* printThread(void * args);
int main(int argc, char **argv)
{
	int port;
	int serverType = 2;
	MultiThreadServer* server1;
	SelectServer* server2;
	EpollServer* server3;

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
	
	
	char filename[] = "test/tests.txt";
	ClientData::Instance()->setFile(filename);

	pthread_t tid;
	pthread_create(&tid, NULL, printThread, (void*)NULL);
	

	switch(serverType){
		case 1:
			server1 = MultiThreadServer::Instance();
			server1->set_port(port);
			server1->run();
			break;
		case 2:
			server2 = SelectServer::Instance();
			server2->set_port(port);
			server2->run();
			break;
		case 3:
		default:
			server3 = EpollServer::Instance();
			server3->set_port(port);
			server3->run();
			break;
	}
	
//	pthread_kill(tid, SIGTERM);
	return 0;
}

void* printThread(void * args){
	
	while(1){
		sleep(1);
		ClientData::Instance()->print();
	}
	return (void*)0;
}


