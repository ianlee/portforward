#include "multi_thread_server.h"
void* printThread(void * args);
int main(int argc, char **argv)
{
	int port;

	switch(argc)
	{
		case 1:
			port = TCP_PORT;
			break;
		case 2:
			port = atoi(argv[1]);
			break;
		default:
			fprintf(stderr, "Usage: %s [port]\n", argv[0]);
			exit(1);
	}
	MultiThreadServer* server = MultiThreadServer::Instance();


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


