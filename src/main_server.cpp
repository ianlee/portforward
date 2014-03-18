/*#include "multi_thread_server.h"
#include "select_server.h"*/
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
	int port = TCP_PORT;
	int serverType = 3;
	int numberWorkers = 10;
	/*MultiThreadServer* server1;
	SelectServer* server2;*/
	EpollServer* server3;
	const char* filename = "test/tests.txt";
	int buflen = 255;
	//signal(SIGINT, signalHandler);  
	//get args
	while ((c = getopt (argc, argv, "f:n:p:t:b:n:")) != -1){
         switch (c){
			case 'p':
				port= atoi(optarg);
				break;
			case 't':
				serverType = atoi(optarg);
				break;
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
				fprintf(stderr, "Usage: %s [-t servertype] [-p port] [-f filename] [-n numberOfWorkers] [-b buflength]\n", argv[0]);
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
	
	//start server
	switch(serverType){
		/*case 1:
			server1 = MultiThreadServer::Instance();
			server1->set_port(port);
			server1->setBufLen(buflen);
			server1->run();
			break;
		case 2:
			server2 = SelectServer::Instance();
			server2->set_port(port);
			server2->setBufLen(buflen);
			server2->set_num_threads(numberWorkers);
			server2->run();
			break;*/
		case 3:
		default:
			/*server3 = EpollServer::Instance();*/
			server3 = new EpollServer;
			server3->set_port(port);
			server3->setBufLen(buflen);
			server3->set_num_threads(numberWorkers);
			server3->run();
			break;
	}
	pthread_exit(NULL);

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
	const struct timespec timeout {0,500000000};

	while(1){
		nanosleep(&timeout, NULL);
		ClientData::Instance()->print();
	}
	return (void*)0;
}
void signalHandler( int signum )
{
	printf("Interupt: %d\n",signum);
	ClientData::Instance()->cleanup(signum);
}
