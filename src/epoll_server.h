#ifndef EPOLL_SERVER_H
#define EPOLL_SERVER_H

#include "client_data.h"
#include "blocking_queue.h"

#include <atomic>
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
#include <sys/epoll.h>
#include <assert.h>
#include <fcntl.h>


#define BUFLEN 255
#define TCP_PORT 7000
#define MAXCLIENTS 1000
#define NUMTHREADS 1



class EpollServer {

public:
	std::chrono::milliseconds timeout= std::chrono::milliseconds(300000);
	static EpollServer* Instance();

	EpollServer(int port);
	int run();
	int create_socket();
	int bind_socket();
	void listen_for_clients();
	int accept_client();
	void send_msgs(int socket, char * data);
	int recv_msgs(int socket, char * bp);
	int set_sock_option(int listenSocket);
	int set_port(int port);
private:

	int 	serverSock, _port;
//	std::vector<client_data*> list_of_clients;
	static void * process_client(void * args);
	static EpollServer* m_pInstance;
	
	blocking_queue<int> fd_queue;
	
	int epoll_fd;
	int maxfd;
	int maxi;
	int nready;
	struct epoll_event events[MAXCLIENTS], event;
};

#endif

