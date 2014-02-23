#ifndef ECHO_CLIENT_H
#define ECHO_CLIENT_H

#include "client_data.h"

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
#include <sys/wait.h>
#include <sys/epoll.h>
#include <assert.h>
#include <fcntl.h>

#define SERVER_TCP_PORT		7000	// Default port
#define MAX_CONNECT		100	// Max number of connections to server

class Client {

public:
	Client(char * host, int port, int t_sent);
	int run();
	void child_client_process(int client_num, int times_sent);
	int create_socket();
	void wait_for_client_processes();
	int connect_to_server(int socket, char * host);
	int send_msgs(int socket, char * data);
	int recv_msgs(int socket, char * buf);
	int setBufLen(int buflen);
	int setConnections(int connections);
private:
	char * _host;
	int _port, times_sent, _buflen, _connections;

};

#endif
