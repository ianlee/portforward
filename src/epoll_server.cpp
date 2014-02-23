#include "epoll_server.h"

/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: epoll_server.cpp - Hold the code for the epoll server used by the echo client. 
--
-- PROGRAM: server
--
-- FUNCTIONS: EpollServer::EpollServer(int port)
--			  EpollServer* EpollServer::Instance()
--			  int EpollServer::run()
--			  int EpollServer::create_socket()
--			  int EpollServer::bind_socket()
--			  void EpollServer::listen_for_clients()
--			  int EpollServer::accept_client()
--			  void EpollServer::send_msgs(int socket, char * data)
--			  int EpollServer::recv_msgs(int socket, char * bp)
--			  int EpollServer::set_sock_option(int listenSocket)
--			  void * EpollServer::process_client(void * args)
--			  int EpollServer::set_port(int port)
--			  int EpollServer::set_num_threads(int num);
--
--
-- DATE: 2014/02/21
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ian Lee, Luke Tao
--
-- PROGRAMMER: Ian Lee, Luke Tao
--
-- NOTES: Epoll server class tested by the echo client.
----------------------------------------------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------------------------------------- 
-- FUNCTION: EpollServer (constructor)
--
-- DATE: 2014/02/21
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ian Lee, Luke Tao
--
-- PROGRAMMER: Ian Lee, Luke Tao
--
-- INTERFACE: EpollServer::EpollServer(int port)
--				       int port - server port
--
-- RETURNS:  N/A
--
-- NOTES: Epoll Server constructor that will initialize the server port.
----------------------------------------------------------------------------------------------------------------------*/
EpollServer::EpollServer(int port) : _port(port) {}


EpollServer* EpollServer::m_pInstance = NULL;

/*-------------------------------------------------------------------------------------------------------------------- 
-- FUNCTION: Instance
--
-- DATE: 2014/02/21
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ian Lee, Luke Tao
--
-- PROGRAMMER: Ian Lee, Luke Tao
--
-- INTERFACE: EpollServer* EpollServer::Instance()
--
-- RETURNS:  Returns the instance of class generated.
--
-- NOTES: Creates an instance of epoll server.
----------------------------------------------------------------------------------------------------------------------*/
EpollServer* EpollServer::Instance()
{
	if (!m_pInstance)   // Only allow one instance of class to be generated.
		m_pInstance = new EpollServer(TCP_PORT);
	return m_pInstance;
}

/*-------------------------------------------------------------------------------------------------------------------- 
-- FUNCTION: run
--
-- DATE: 2014/02/21
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ian Lee, Luke Tao
--
-- PROGRAMMER: Ian Lee, Luke Tao
--
-- INTERFACE: int EpollServer::run()
--
-- RETURNS:  0 on success
--
-- NOTES: Main epoll server function
----------------------------------------------------------------------------------------------------------------------*/
int EpollServer::run() {
	pthread_t tids[_numThreads];
	int i;
	
	for(int i = 0; i < _numThreads; i++)
	{
		pthread_create(&tids[i], NULL, process_client, NULL);
	}
	
	
	
	serverSock = create_socket();
	serverSock = bind_socket();
	serverSock = set_sock_option(serverSock);
	

	// Make the server listening socket non-blocking
	if (fcntl (serverSock, F_SETFL, O_NONBLOCK | fcntl (serverSock, F_GETFL, 0)) == -1) 
		fprintf(stderr,"fcntl\n");
	listen_for_clients();	
	maxfd = serverSock;
	maxi = -1;
	
	epoll_fd = epoll_create(MAXCLIENTS);
	if (epoll_fd == -1) 
		fprintf(stderr,"epoll_create\n");
	// Add the server socket to the epoll event loop
	event.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLET;
	event.data.fd = serverSock;
	if (epoll_ctl (epoll_fd, EPOLL_CTL_ADD, serverSock, &event) == -1) 
		fprintf(stderr,"epoll_ctl\n");
	
	while(true){
		
		nready = epoll_wait (epoll_fd, events, MAXCLIENTS, -1);
		for (i = 0; i < nready; i++){	// check all clients for data

		// Case 1: Error condition
    		if (events[i].events & (EPOLLHUP | EPOLLERR)) {
				fputs("epoll: EPOLLERR", stderr);
				int sock = events[i].data.fd;
				close(sock);
				ClientData::Instance()->removeClient(sock);
				continue;
    		}
    		assert (events[i].events & EPOLLIN);

	    	// Case 2: Server is receiving a connection request
	    	if (events[i].data.fd == serverSock) {
				accept_client();				
				continue;
    		}

    		// Case 3: One of the sockets has read data

			fd_queue.push(events[i].data.fd, timeout);

 		}
	
	}
	close(serverSock);
	return 0;
}

/*-------------------------------------------------------------------------------------------------------------------- 
-- FUNCTION: create_socket
--
-- DATE: 2014/02/21
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ian Lee, Luke Tao
--
-- PROGRAMMER: Ian Lee, Luke Tao
--
-- INTERFACE: int EpollServer::create_socket()
--
-- RETURNS:  Socket Descriptor
--
-- NOTES: Creates a socket and returns the socket descriptor on successful creation.
----------------------------------------------------------------------------------------------------------------------*/
int EpollServer::create_socket()
{
	int sd;
	// Create the socket
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("Cannot create socket");
	}
	return sd;
}

/*-------------------------------------------------------------------------------------------------------------------- 
-- FUNCTION: bind_socket
--
-- DATE: 2014/02/21
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ian Lee, Luke Tao
--
-- PROGRAMMER: Ian Lee, Luke Tao
--
-- INTERFACE: int EpollServer::bind_socket()
--
-- RETURNS:  Server Socket Descriptor
--
-- NOTES: Function that binds an address to the server socket and returns the server socket.
----------------------------------------------------------------------------------------------------------------------*/
int EpollServer::bind_socket()
{
	struct	sockaddr_in server;

	// Bind an address to the socket
	bzero((char *)&server, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(_port);
	server.sin_addr.s_addr = htonl(INADDR_ANY); // Accept connections from any client

	if (bind(serverSock, (struct sockaddr *)&server, sizeof(server)) == -1)
	{
		perror("Can't bind name to socket");
		exit(1);
	}
	return serverSock;
}

/*-------------------------------------------------------------------------------------------------------------------- 
-- FUNCTION: listen_for_clients
--
-- DATE: 2014/02/21
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ian Lee, Luke Tao
--
-- PROGRAMMER: Ian Lee, Luke Tao
--
-- INTERFACE: void EpollServer::listen_for_clients()
--
-- RETURNS:  void
--
-- NOTES: Sets the number of clients the server will handle requests to.
----------------------------------------------------------------------------------------------------------------------*/
void EpollServer::listen_for_clients()
{
	// Listen for connections
	
	listen(serverSock, SOMAXCONN);
}

/*-------------------------------------------------------------------------------------------------------------------- 
-- FUNCTION: accept_client
--
-- DATE: 2014/02/21
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ian Lee, Luke Tao
--
-- PROGRAMMER: Ian Lee, Luke Tao
--
-- INTERFACE: int EpollServer::accept_client()
--
-- RETURNS:  New Socket Descriptor
--
-- NOTES: Function that blocks until a client connection request comes in. It will add the client to the list.
----------------------------------------------------------------------------------------------------------------------*/
int EpollServer::accept_client()
{
	
	struct	sockaddr_in client;
	unsigned int client_len = sizeof(client);
	int sServerSock;
	if ((sServerSock = accept (serverSock, (struct sockaddr *)&client, &client_len)) == -1){
		if (errno != EAGAIN && errno != EWOULDBLOCK) {
			fprintf(stderr, "Can't accept client\n");
			return 0;
		}
		
	}
	
	// Make the fd_new non-blocking
	if (fcntl (sServerSock, F_SETFL, O_NONBLOCK | fcntl(sServerSock, F_GETFL, 0)) == -1) {
		fprintf(stderr,"fcntl\n");
	}
	// Add the new socket descriptor to the epoll loop
	event.data.fd = sServerSock;
	if (epoll_ctl (epoll_fd, EPOLL_CTL_ADD, sServerSock, &event) == -1) {
		fprintf(stderr,"epoll_ctl\n");
	}
	
	
	ClientData::Instance()->addClient(sServerSock, inet_ntoa(client.sin_addr),client.sin_port );
	printf(" Remote Address:  %s\n", inet_ntoa(client.sin_addr));
	return sServerSock;
}

/*-------------------------------------------------------------------------------------------------------------------- 
-- FUNCTION: send_msgs
--
-- DATE: 2014/02/21
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ian Lee, Luke Tao
--
-- PROGRAMMER: Ian Lee, Luke Tao
--
-- INTERFACE: void EpollServer::send_msgs(int socket, char * data)
--					  int socket - server sock
--					  char * data - data that the server will send back to the client
--
-- RETURNS:  void
--
-- NOTES: Send Messages function used by the epoll server.
----------------------------------------------------------------------------------------------------------------------*/
void EpollServer::send_msgs(int socket, char * data)
{
	send(socket, data, BUFLEN, 0);
}

/*-------------------------------------------------------------------------------------------------------------------- 
-- FUNCTION: recv_msgs
--
-- DATE: 2014/02/21
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ian Lee, Luke Tao
--
-- PROGRAMMER: Ian Lee, Luke Tao
--
-- INTERFACE: int EpollServer::recv_msgs(int socket, char * bp)
--					 int socket - server socket
--					 char * bp - data that the server will receive from the client
--
-- RETURNS:  Socket Descriptor
--
-- NOTES: Send Messages function used by the epoll server.
----------------------------------------------------------------------------------------------------------------------*/
int EpollServer::recv_msgs(int socket, char * bp)
{
	int n, bytes_to_read = BUFLEN;
	while ((n = recv (socket, bp, bytes_to_read, 0)) < bytes_to_read)
	{
		
		if(n == -1){
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				break;
			}
			printf("error %d %d %d\n", bytes_to_read, n, socket);
			printf("error %d\n",errno);
			ClientData::Instance()->removeClient(socket);
			close(socket);
			return -1;
		} else if (n == 0){
			printf("socket was gracefully closed by other side %d\n",socket);
			ClientData::Instance()->removeClient(socket);
			close(socket);
			return -1;
		}
		bp += n;
		bytes_to_read -= n;
	}

	return 0;
}

/*-------------------------------------------------------------------------------------------------------------------- 
-- FUNCTION: set_sock_option
--
-- DATE: 2014/02/21
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ian Lee, Luke Tao
--
-- PROGRAMMER: Ian Lee, Luke Tao
--
-- INTERFACE: int EpollServer::set_sock_option(int listenSocket)
--					       int listenSocket - listening socket
--
-- RETURNS:  N/A
--
-- NOTES: Function that sets the listening socket options.
----------------------------------------------------------------------------------------------------------------------*/
int EpollServer::set_sock_option(int listenSocket)
{
	// Reuse address set
	int value = 1;
	if (setsockopt (listenSocket, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value)) == -1)
		perror("setsockopt failed\n");
	
	// Set buffer length to send or receive to BUFLEN.
	value = BUFLEN;
	if (setsockopt (listenSocket, SOL_SOCKET, SO_SNDBUF, &value, sizeof(value)) == -1)
		perror("setsockopt failed\n");
	
	if (setsockopt (listenSocket, SOL_SOCKET, SO_RCVBUF, &value, sizeof(value)) == -1)
		perror("setsockopt failed\n");

	return listenSocket;

}

/*-------------------------------------------------------------------------------------------------------------------- 
-- FUNCTION: process_client
--
-- DATE: 2014/02/21
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ian Lee, Luke Tao
--
-- PROGRAMMER: Ian Lee, Luke Tao
--
-- INTERFACE: void * EpollServer::process_client(void * args)
--
-- RETURNS:  0 on success
--
-- NOTES: Thread that processes the client by receiving and sending packets from the server. 
----------------------------------------------------------------------------------------------------------------------*/
void * EpollServer::process_client(void * args)
{	
	int sock;
	char buf[BUFLEN];
	EpollServer* mServer = EpollServer::Instance();
	
	while(1){
		mServer->fd_queue.pop(sock, mServer->timeout);
		if(!ClientData::Instance()->has(sock)){
			continue;
		}
		
		if(mServer->recv_msgs(sock, buf)<0){
			continue;
		}
		std::cout << "set rtt: " << ClientData::Instance()->setRtt(sock) << std::endl;
		//printf("Received: %s\n", buf);	
		mServer->send_msgs(sock, buf);	
	}		            				
	return (void*)0;

}

/*-------------------------------------------------------------------------------------------------------------------- 
-- FUNCTION: set_port
--
-- DATE: 2014/02/21
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ian Lee, Luke Tao
--
-- PROGRAMMER: Ian Lee, Luke Tao
--
-- INTERFACE: int EpollServer::set_port(int port)
--					int port - server port specified
--
-- RETURNS:  N/A
--
-- NOTES: Sets server port when starting the server.
----------------------------------------------------------------------------------------------------------------------*/
int EpollServer::set_port(int port){
	_port = port;
	return 1;
}
/*-------------------------------------------------------------------------------------------------------------------- 
-- FUNCTION: set_num_threads
--
-- DATE: 2014/02/21
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ian Lee, Luke Tao
--
-- PROGRAMMER: Ian Lee, Luke Tao
--
-- INTERFACE: int EpollServer::set_num_threads(int num)
--					int num - number of worker threads server should use
--
-- RETURNS:  N/A
--
-- NOTES: Sets server port when starting the server.
----------------------------------------------------------------------------------------------------------------------*/
int EpollServer::set_num_threads(int num){
	_numThreads=num;
	return 1;
}