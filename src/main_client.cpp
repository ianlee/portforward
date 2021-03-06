#include "echo_client.h"
#include <time.h>
void* printThread(void * args);
void signalHandler( int signum );
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
	char *host= NULL;
	int port = SERVER_TCP_PORT;
	int times_sent = 5000;
	char c;
	int buflen = 255;
	int connections = 1000;
	const char* filename = "test/client.txt";
	signal(SIGINT, signalHandler);  
	while ((c = getopt (argc, argv, "a:p:t:b:c:")) != -1){
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
				break;
			case 'c':
				connections = atoi(optarg);
				break;
			case '?':
			default:
				fprintf(stderr, "Usage: %s [-a hostname] [-p port] [-t timesToSend] [-c maxConnect] [-b buflength]\n", argv[0]);
				exit(1);
		}
	}

	//set filename
	if(ClientData::Instance()->setFile(filename) <0 ){
		fprintf(stderr, "File could not be opened: %s\n", filename);
		exit(1);
	}
	//create stat printing thread
	pthread_t tid;
	pthread_create(&tid, NULL, printThread, (void*)NULL);
	
	printf("host:%s\n", host);
	Client client(host, port, times_sent);
	client.setBufLen(buflen);
	client.setConnections(connections);
	client.run();
	return 0;
}


/*-------------------------------------------------------------------------------------------------------------------- 
-- FUNCTION: printThread
--
-- DATE: 2014/02/21
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ian Lee, Luke Tao
--
-- PROGRAMMER: Ian Lee, Luke Tao
--
-- INTERFACE: void* printThread(void * args)
--
-- RETURNS:  0 on success
--
-- NOTES: Thread that prints the number of clients to a file in a loop.
----------------------------------------------------------------------------------------------------------------------*/

void* printThread(void * args){
	const struct timespec timeout {1,0};

	while(1){
		nanosleep(&timeout, NULL);
		ClientData::Instance()->print();
	}
	return (void*)0;
}

void signalHandler( int signum )
{
	ClientData::Instance()->cleanup(signum);
}
