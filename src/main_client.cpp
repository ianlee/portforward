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
	int times_sent = 10000;
	char c;
	
	while ((c = getopt (argc, argv, "aptfn:")) != -1){
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
			
			case '?':
			default:
				fprintf(stderr, "Usage: %s [-a hostname] [-p port] [-t timesToSend] \n", argv[0]);
				exit(1);
		}
	}

	Client client(host, port, times_sent);
	client.run();
	return 0;
}
