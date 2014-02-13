#include "echo_client.h"

int main(int argc, char **argv)
{
	char *host;
	int port;

	switch (argc)
	{
		case 2:
			host = argv[1];
			port = SERVER_TCP_PORT;
			break;
		case 3:
			host = argv[1];
			port = atoi(argv[2]);
			break;
		default:
			fprintf(stderr, "Usage: %s host [port]\n", argv[0]);
			exit(1);	
	}
	Client client(host, port);
	client.run();
	return 0;
}
