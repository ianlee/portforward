#include "echo_client.h"

int main(int argc, char **argv)
{
	char *host;
	int port, times_sent;

	switch (argc)
	{
		case 3:
			host = argv[1];
			port = SERVER_TCP_PORT;
			times_sent = atoi(argv[2]);
			break;
		case 4:
			host = argv[1];
			port = atoi(argv[2]);
			times_sent = atoi(argv[3]);
			break;
		default:
			fprintf(stderr, "Usage: %s host [port] <number of times sent>\n", argv[0]);
			exit(1);	
	}
	Client client(host, port);
	client.run();
	return 0;
}
