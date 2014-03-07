#ifndef EPOLL_SERVER_H
#define EPOLL_SERVER_H

#include "client_data.h"
#include "blocking_queue.h"
#include "config.h"
#include "pair_sockets.h"

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
#define MAXCLIENTS 100000



class EpollServer {

public:
	std::chrono::milliseconds timeout= std::chrono::milliseconds(3000000);
	static EpollServer* Instance();


	int run();
	int create_socket();
	int bind_socket(int socket, int port);
	void listen_for_clients(int socket);
	int accept_client(int socket);
	void send_msgs(int socket, char * data);
	int recv_msgs(int socket, char * bp);
	int removeSocket(int socket);
	int set_sock_option(int listenSocket);
	int set_port(int port);
	int set_num_threads(int num);
	int setBufLen(int buflen);
	int _buflen;
	
	int create_listen_sockets();
	int connect_to_dest(int sSocket);
	PairSockets pairSock;
private:

	int 	/*serverSock,*/ _port, _numThreads;
	static void * process_client(void * args);
	static void * epoll_loop(void* args);
	
	blocking_queue<int> fd_queue;
	
	int epoll_fd;
	int epoll_client_fd;
	Config* conf;

	
	
	//int nready;

//	struct epoll_event events[MAXCLIENTS], event;
};

struct epoll_loop_struct {
	int fd;
	EpollServer* inst;
	int type;
};
#endif

