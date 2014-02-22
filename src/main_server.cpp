#include "multi_thread_server.h"
#include "select_server.h"
#include "epoll_server.h"
#include <time.h>
void* printThread(void * args);

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
	int port = TCP_PORT;
	int serverType = 3;
	int numberWorkers = 10;
	MultiThreadServer* server1;
	SelectServer* server2;
	EpollServer* server3;
	char filename[] = "test/tests.txt";
	//get args
	while ((c = getopt (argc, argv, "pt:")) != -1){
         switch (c){
			case 'p':
				port= optarg;
				break;
			case 't':
				serverType = optarg;
				break;
			case 'f':
				filename = optarg;
				break;
			case 'n':
				numberWorkers= optarg;
				break;
			case '?':
			default:
				fprintf(stderr, "Usage: %s [-t servertype] [-p port] [-f filename]\n", argv[0]);
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
		case 1:
			server1 = MultiThreadServer::Instance();
			server1->set_port(port);
			server1->run();
			break;
		case 2:
			server2 = SelectServer::Instance();
			server2->set_port(port);
			server2->set_num_threads(numberWorkers);
			server2->run();
			break;
		case 3:
		default:
			server3 = EpollServer::Instance();
			server3->set_port(port);
			server3->set_num_threads(numberWorkers);
			server3->run();
			break;
	}
	
//	pthread_kill(tid, SIGTERM);
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
	struct timespec timeout;
	timeout.tv_sec=0;
	timeout.tv_nsec=500000000; // 0.5seconds
	while(1){
		nano_sleep(timeout, NULL);
		ClientData::Instance()->print();
	}
	return (void*)0;
}
