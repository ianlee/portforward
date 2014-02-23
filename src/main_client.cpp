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
--		       int argc - number of cmd-line arguments
--		       char **argv - double pointer to array of arguments
--
-- RETURNS:  0 on success
--
-- NOTES: Main function that parses user-specified command-line arguments and starts echo client.
----------------------------------------------------------------------------------------------------------------------*/
int main(int argc, char **argv)
{
	char *host;
	int port = SERVER_TCP_PORT;
	int times_sent = 5000;
	char c;
	int buflen = 255;
	int connections = 1000;
	
	while ((c = getopt (argc, argv, "aptbc:")) != -1){
         switch (c){
			case 'p':
				port= atoi(optarg);
				break;
			case 'a':
				host = optarg;
				break;
			case 't':
				times_sent = atoi(optarg);
				break;
			case 'b':
				buflen = atoi(optarg);
			case 'c':
				connections = atoi(optarg);
			case '?':
			default:
				fprintf(stderr, "Usage: %s [-a hostname] [-p port] [-t timesToSend] [-c maxConnect] [-b buflength]\n", argv[0]);
				exit(1);
		}
	}

	Client client(host, port, times_sent);
	client.setBufLen(buflen);
	client.setConnections(connections);
	client.run();
	return 0;
}
