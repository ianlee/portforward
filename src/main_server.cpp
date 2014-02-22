#include "multi_thread_server.h"
#include "select_server.h"
#include "epoll_server.h"
#include <time.h>
void* printThread(void * args);
int main(int argc, char **argv)
{
	int port;
	int serverType = 3;
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
void getArgs(int argc, char** argv){
	while ((c = getopt (argc, argv, "pt:")) != -1){
         switch (c){
		}
	}
	
}
void* printThread(void * args){
	struct timespec timeout;
	timeout.tv_sec=0;
	timeout.tv_nsec=500000000; // 0.5seconds
	while(1){
		nano_sleep(timeout, NULL);
		ClientData::Instance()->print();
	}
	return (void*)0;
}


