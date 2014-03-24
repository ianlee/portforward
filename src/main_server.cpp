
#include "epoll_server.h"
#include <time.h>
void* printThread(void * args);
void signalHandler( int signum );

/*-------------------------------------------------------------------------------------------------------------------- 
-- FUNCTION: main (server)
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
-- NOTES: Main function that parses command-line arguments and starts a type of server, depending on the user's choice.
----------------------------------------------------------------------------------------------------------------------*/
int main(int argc, char **argv)
{
	char c;
	
	int serverType = 3;
	int numberWorkers = 1;
	
	EpollServer* server3;
	const char* filename = "test/tests.txt";
	int buflen = 255;
	
	//get args
	while ((c = getopt (argc, argv, "f:n:t:b:n:")) != -1){
         switch (c){
			
			
			case 'f':
				filename = optarg;
				break;
			case 'n':
				numberWorkers= atoi(optarg);
				break;
			case 'b':
				buflen = atoi(optarg);
				break;
			case '?':
			default:
				fprintf(stderr, "Usage: %s [-f filename] [-n numberOfWorkers] [-b buflength]\n", argv[0]);
				exit(1);
		}
	}
	
	//set filename
	if(ClientData::Instance()->setFile(filename) <0 ){
		fprintf(stderr, "File could not be opened: %s\n", filename);
		exit(1);
	}
	
	
	//start server
	switch(serverType){
		
		case 3:
		default:
			
			server3 = new EpollServer;
			
			server3->setBufLen(buflen);
			server3->set_num_threads(numberWorkers);
			server3->run();
			break;
	}
	pthread_exit(NULL);

	return 0;
}


