#ifndef ECHO_CLIENT_H
#define ECHO_CLIENT_H

#include <iostream>
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

#define SERVER_TCP_PORT		7000	// Default port
#define BUFLEN			255  	// Buffer length

class Client {

public:
	Client(char * host, int port);
	int run();
	int create_socket();
	int connect_to_server(int socket, char * host);
	int send_msgs(int socket, char * data);
	int recv_msgs(int socket, char * buf);

private:
	char * _host;
	int clientSock, _port;		
	pthread_t tid;

};

#endif
