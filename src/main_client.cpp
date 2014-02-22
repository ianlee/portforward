#include "echo_client.h"

/*-------------------------------------------------------------------------------------------------------------------- 
-- FUNCTION: main (client)
--
-- DATE: 2014/02/21
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ian Lee, Luke Tao
--
-- PROGRAMMER: Ian Lee, Luke Tao
--
-- INTERFACE: int main(int argc, char **argv)
--					   int argc - number of cmd-line arguments
--					   char **argv - double pointer to array of arguments
--
-- RETURNS:  0 on success
--
-- NOTES: Main function that parses user-specified command-line arguments and starts echo client.
----------------------------------------------------------------------------------------------------------------------*/
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
	Client client(host, port, times_sent);
	client.run();
	return 0;
}
