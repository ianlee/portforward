#ifndef MULTI_THREAD_SERVER_H
#define MULTI_THREAD_SERVER_H

#include <iostream>
#include <vector>
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdlib.h>
#include <strings.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>

#define BUFLEN 255
#define TCP_PORT 7000
#define MAXCLIENTS 1000

class MultiThreadServer {

public:
	MultiThreadServer(int port);
	int run();
	int create_socket();
	int bind_socket();
	void listen_for_clients();
	int accept_client();
	void send_msgs(int socket, char * data);
	int recv_msgs(int socket, char * bp);
	int set_sock_option(int listenSocket);
private:

	int 	serverSock, newServerSock, _port;
	std::vector<client_data*> list_of_clients;
	static void * process_client(void * args);
	
};

#endif

