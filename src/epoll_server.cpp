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
--			  int EpollServer::recv_msgs(int so+cket, char * bp)
--			  int EpollServer::set_sock_option(int listenSocket)
--			  void * EpollServer::process_client(void * args)
--			  int EpollServer::set_port(int port)
--			  int EpollServer::set_num_threads(int num)
--			  int EpollServer::setBufLen(int buflen)
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
//EpollServer::EpollServer(int port) : _port(port) {}


//EpollServer* EpollServer::m_pInstance = NULL;

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
/*EpollServer* EpollServer::Instance()
{
	static EpollServer m_pInstance;

	return &m_pInstance;
}*/

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
	pthread_t listenThread;
	pthread_t clientThread;
	epoll_loop_struct listenData;
	epoll_loop_struct clientData;
	int i;

	
	
	
	create_listen_sockets();
	epoll_client_fd = epoll_create(MAXCLIENTS);
	if (epoll_client_fd == -1) 
		fprintf(stderr,"epoll_create\n");
	/*epoll loop should be in multiple threads: 1 for listen sockets, 1 for client sockets*/
	listenData.inst = this;
	clientData.inst = this;
	listenData.fd = epoll_fd;
	clientData.fd = epoll_client_fd;
	listenData.type = 1;
	clientData.type = 2;
	
	
	for(i = 0; i < _numThreads; i++)
	{
		pthread_create(&tids[i], NULL, process_client,(void*) this);
	}
	pthread_create(&listenThread, NULL, epoll_loop, (void*) &listenData);
	pthread_create(&clientThread, NULL, epoll_loop, (void*) &clientData);
/*	epoll_loop(epoll_fd);
	epoll_loop(epoll_client_fd);*/
	sleep(1);

	return 0;
}


int EpollServer::create_listen_sockets(){
	conf = new Config;
	conf->setFilename("./config.txt");
	conf->parseFile();
	
	struct epoll_event event;
	int port;
	int socket;
	
	epoll_fd = epoll_create(MAXCLIENTS);
	if (epoll_fd == -1) 
		fprintf(stderr,"epoll_create\n");
	
	while((port = conf->getPort() )){

		socket = create_socket();
		socket = bind_socket(socket, port);
		socket = set_sock_option(socket);
		conf->storeSocketIntoMap(port, socket);
    	printf("create socket %d for port %d\n", socket, port);
		// Make the server listening socket non-blocking
		if (fcntl (socket, F_SETFL, O_NONBLOCK | fcntl (socket, F_GETFL, 0)) == -1) 
			fprintf(stderr,"fcntl\n");
		listen_for_clients(socket);	
		
		// Add the server socket to the epoll event loop
		event.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLET;
		event.data.fd = socket;
		if (epoll_ctl (epoll_fd, EPOLL_CTL_ADD, socket, &event) == -1) 
			fprintf(stderr,"epoll_ctl\n");
	}
	return epoll_fd;
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
-- INTERFACE: int EpollServer::bind_socket(int socket, int port)
--
-- RETURNS:  Server Socket Descriptor
--
-- NOTES: Function that binds an address to the server socket and returns the server socket.
----------------------------------------------------------------------------------------------------------------------*/
int EpollServer::bind_socket(int socket, int port)
{
	struct	sockaddr_in server;

	// Bind an address to the socket
	bzero((char *)&server, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = htonl(INADDR_ANY); // Accept connections from any client

	if (bind(socket, (struct sockaddr *)&server, sizeof(server)) == -1)
	{
		perror("Can't bind name to socket");
		exit(1);
	}
	return socket;
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
void EpollServer::listen_for_clients(int socket)
{
	// Listen for connections
	
	listen(socket, SOMAXCONN);
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
int EpollServer::accept_client(int socket)
{
	struct epoll_event event;
	struct	sockaddr_in client;
	unsigned int client_len = sizeof(client);
	int sSocket, dSocket;
	if ((sSocket = accept (socket, (struct sockaddr *)&client, &client_len)) == -1){
		if (errno != EAGAIN && errno != EWOULDBLOCK) {
			fprintf(stderr, "Can't accept client\n");
			return -1;
		} else {
			return 0;
		}
		
	}

	// Make the fd_new non-blocking
	if (fcntl (sSocket, F_SETFL, O_NONBLOCK | fcntl(sSocket, F_GETFL, 0)) == -1) {
		fprintf(stderr,"fcntl\n");
	}
	//create socket connecting to destination
	dSocket = connect_to_dest(socket);
	// Add the new socket descriptors to the epoll loop
	event.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLET;
	event.data.fd = sSocket;
	if (epoll_ctl (epoll_client_fd, EPOLL_CTL_ADD, sSocket, &event) == -1) {
		fprintf(stderr,"epoll_ctl\n");
	}
	event.data.fd = dSocket;
	if (epoll_ctl (epoll_client_fd, EPOLL_CTL_ADD, dSocket, &event) == -1) {
		fprintf(stderr,"epoll_ctl\n");
	}
	
	//add both sockets to current connection list. //TODO::change this to client list.
	pairSock.insertPairOfSockets(sSocket, dSocket);
	
	ClientData::Instance()->addClient(sSocket, inet_ntoa(client.sin_addr),client.sin_port );

	return sSocket;
}
int EpollServer::connect_to_dest(int sSocket)
{
	struct sockaddr_in server;
	struct hostent	*hostptr;
	int socket = create_socket();
	DestData destInfo;
	if( !conf->getData(sSocket, &destInfo) ) {
	    exit(1);
	}

	bzero((char *)&server, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(destInfo.destPort);
	if ((hostptr = gethostbyname(destInfo.destAddr)) == NULL)
	{
		fprintf(stderr, "Unknown server address\n");
		exit(1);
	}
	bcopy(hostptr->h_addr, (char *)&server.sin_addr, hostptr->h_length);

	if (connect (socket, (struct sockaddr *)&server, sizeof(server)) == -1)
	{
		fprintf(stderr, "Can't connect to server\n");
		fflush(stderr);
		perror("connect");
		exit(1);
		return 0;
	}
	if(socket > 0){
		ClientData::Instance()->addClient(socket, inet_ntoa(server.sin_addr),server.sin_port );
	}

	return socket;
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
void EpollServer::send_msgs(int socket, char * data, int blen)
{
	send(socket, data, blen, 0);
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
	int n, bytes_to_read = _buflen;
	int blen = 0;
	while ((n = recv (socket, bp, bytes_to_read, 0)) < bytes_to_read)
	{
		
		if(n == -1){
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				break;
			}
			printf("error %d %d %d\n", bytes_to_read, n, socket);
			printf("error %d\n",errno);
			ClientData::Instance()->removeClient(socket);
			removeSocket(socket);
			//close(socket);
			return -1;
		} else if (n == 0){
			printf("socket was gracefully closed by other side %d\n",socket);
			ClientData::Instance()->removeClient(socket);
			removeSocket(socket);
//			close(socket);
			return -1;
		}
		bp += n;
		bytes_to_read -= n;
		blen+=n;
	}
	if(n>0){
		blen +=n;
	}

	return blen;
}

int EpollServer::removeSocket(int socket){
    close(socket);
    close(pairSock.getSocketFromList(socket));
    return pairSock.removeSocketFromList(socket);
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
	
	// Set buffer length to send or receive to _buflen.
	value = _buflen;
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
	int dsock;
	EpollServer* mServer = (EpollServer*) args;
	/*EpollServer* mServer = EpollServer::Instance();*/
	char buf[mServer->_buflen];	
	while(1){
		mServer->fd_queue.pop(sock, mServer->timeout);
	/*	if(!ClientData::Instance()->has(sock)){
			continue;
		}*/
		dsock = mServer->pairSock.getSocketFromList(sock);
		if(dsock ==-1) continue;
		if(mServer->recv_msgs(sock, buf)<0){
			continue;
		}

//		ClientData::Instance()->setRtt(sock);
		mServer->send_msgs(dsock, buf);	
//		ClientData::Instance()->recordData(sock, mServer->_buflen);
	}		          				
	return (void*)0;

}

void* EpollServer::epoll_loop(void * args){

	epoll_loop_struct* epollstruct = (epoll_loop_struct*) args;
	int local_epoll_fd = epollstruct->fd;
	EpollServer* mServer = epollstruct->inst;
	int type = epollstruct->type;
	int i;
	int nready;
	struct epoll_event mevents[MAXCLIENTS];
	
	while(true){
		nready = epoll_wait (local_epoll_fd, mevents, MAXCLIENTS, -1);
		for (i = 0; i < nready; i++){	// check all clients for data

			// Case 1: Error condition
	    		if (mevents[i].events & (EPOLLHUP)) {
				fputs("epoll: EPOLLHUP", stderr);
				int sock = mevents[i].data.fd;
				//close(sock);
				mServer->removeSocket(sock);
				ClientData::Instance()->removeClient(sock);
				continue;
	    		}
	    		if (mevents[i].events & ( EPOLLERR)) {
				fputs("epoll: EPOLLERR", stderr);
				int sock = mevents[i].data.fd;
				//close(sock);
				mServer->removeSocket(sock);
				ClientData::Instance()->removeClient(sock);
				continue;
	    		}
	
	    		assert (mevents[i].events & EPOLLIN);

		    	// Case 2: Server is receiving a connection request
			//if (mevents[i].data.fd == mServer->serverSock) {
            if(type==1){
                printf("data type:%d socket: %d\n", type, mevents[i].data.fd);
				while(mServer->accept_client(mevents[i].data.fd)>1);
				continue;
	    		}

	    		// Case 3: One of the sockets has read data
            //printf("data type:%d socket: %d\n", type, mevents[i].data.fd);
			mServer->fd_queue.push(mevents[i].data.fd, mServer->timeout);

 		}
	
	}
	return (void*) 0;
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
/*-------------------------------------------------------------------------------------------------------------------- 
-- FUNCTION: setBufLen
--
-- DATE: 2014/02/21
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ian Lee, Luke Tao
--
-- PROGRAMMER: Ian Lee, Luke Tao
--
-- INTERFACE: int EpollServer::setBufLen(int buflen)
--					int buflen - buffer length
--
-- RETURNS:  N/A
--
-- NOTES: sets buffer length for recv functions
----------------------------------------------------------------------------------------------------------------------*/
int EpollServer::setBufLen(int buflen){
	_buflen = buflen;
	return 1;
}
