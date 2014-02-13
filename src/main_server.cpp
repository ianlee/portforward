#include "multi_thread_server.h"

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
	MultiThreadServer::Instance();
	server.run();
	return 0;
}
