#include "select_server.h"

/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: select_server.cpp - Hold the code for the select server used by the echo client. 
--
-- PROGRAM: server
--
-- FUNCTIONS: SelectServer::SelectServer(int port)
--			  SelectServer* SelectServer::Instance()
--			  int SelectServer::run()
--			  int SelectServer::create_socket()
--			  int SelectServer::bind_socket()
--			  void SelectServer::listen_for_clients()
--			  int SelectServer::accept_client()
--			  void SelectServer::send_msgs(int socket, char * data)
--			  int SelectServer::recv_msgs(int socket, char * bp)
--			  int SelectServer::set_sock_option(int listenSocket)
--			  void * SelectServer::process_client(void * args)
--			  int SelectServer::set_port(int port)
--			  int SelectServer::set_num_threads(int num);
--			  int SelectServer::setBufLen(int buflen)
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
-- NOTES: Select server class tested by the echo client.
----------------------------------------------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------------------------------------- 
-- FUNCTION: SelectServer (constructor)
--
-- DATE: 2014/02/21
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ian Lee, Luke Tao
--
-- PROGRAMMER: Ian Lee, Luke Tao
--
-- INTERFACE: SelectServer::SelectServer(int port)
--					 int port - server port
--
-- RETURNS:  N/A
--
-- NOTES: Select Server constructor that will initialize the server port.
----------------------------------------------------------------------------------------------------------------------*/
//SelectServer::SelectServer(int port) : _port(port) {}

//SelectServer* SelectServer::m_pInstance = NULL;

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
-- INTERFACE: SelectServer* SelectServer::Instance()
--
-- RETURNS:  Returns the instance of class generated.
--
-- NOTES: Creates an instance of select server.
----------------------------------------------------------------------------------------------------------------------*/
SelectServer* SelectServer::Instance()
{
	static SelectServer m_pInstance;
	//if (!m_pInstance)   // Only allow one instance of class to be generated.
	//	m_pInstance = new SelectServer(TCP_PORT);
	return &m_pInstance;
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
-- INTERFACE: int SelectServer::run()
--
-- RETURNS:  0 on success
--
-- NOTES: Main select server function
----------------------------------------------------------------------------------------------------------------------*/
int SelectServer::run()
{
	//int socks [MAXCLIENTS];
	pthread_t tids[_numThreads];
	int i;
	
	
	for(int i = 0; i < _numThreads; i++)
	{
		pthread_create(&tids[i], NULL, process_client, NULL);
	}
	
	
	
	serverSock = create_socket();
	serverSock = bind_socket();
	serverSock = set_sock_option(serverSock);
	listen_for_clients();

	maxfd = serverSock;
	maxi = -1;
	for (i = 0; i < MAXCLIENTS; i++){
		client[i] = -1;   
	}
	FD_ZERO(&allset);
   	FD_SET(serverSock, &allset);	
	while(true){
		rset = allset;
		nready = select ( maxfd + 1, &rset, NULL, NULL, NULL);
		if (FD_ISSET(serverSock, &rset)) {
			//new connection

			int sock = accept_client();
			if(sock <=0){
				break;
			}
			for (i = 0; i < MAXCLIENTS; i++){
				if (client[i] < 0){
					client[i] = sock;	// save descriptor
					break;
	    			}
			}
			if (i == FD_SETSIZE){
				printf ("Too many clients\n");
				break;
    			}
			FD_SET (sock, &allset);     // add new descriptor to set
			if (sock > maxfd){
				maxfd = sock;	// for select
			}
			if (i > maxi){
				maxi = i;	// new max index in client[] array
			}
			if (--nready <= 0){
				continue;
			}
		
			
		}
		for (i = 0; i <= maxi; i++){	// check all clients for data
     			int sockfd;
			if ((sockfd = client[i]) < 0){
				continue;
			}
			if (FD_ISSET(sockfd, &rset)) {
				fd_queue.push(sockfd, timeout);
	         		if (--nready <= 0){
					break;        // no more readable descriptors
				}
			}
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
-- INTERFACE: int SelectServer::create_socket()
--
-- RETURNS:  Socket Descriptor
--
-- NOTES: Creates a socket and returns the socket descriptor on successful creation.
----------------------------------------------------------------------------------------------------------------------*/
int SelectServer::create_socket()
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
-- INTERFACE: int SelectServer::bind_socket()
--
-- RETURNS:  Server Socket Descriptor
--
-- NOTES: Function that binds an address to the server socket and returns the server socket.
----------------------------------------------------------------------------------------------------------------------*/
int SelectServer::bind_socket()
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
-- INTERFACE: void SelectServer::listen_for_clients()
--
-- RETURNS:  void
--
-- NOTES: Sets the number of clients the server will handle requests to.
----------------------------------------------------------------------------------------------------------------------*/
void SelectServer::listen_for_clients()
{
	// Listen for connections
	
	listen(serverSock, 5);
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
-- INTERFACE: int SelectServer::accept_client()
--
-- RETURNS:  New Socket Descriptor
--
-- NOTES: Function that blocks until a client connection request comes in. It will add the client to the list.
----------------------------------------------------------------------------------------------------------------------*/
int SelectServer::accept_client()
{
	
	struct	sockaddr_in client;
	unsigned int client_len = sizeof(client);
	int sServerSock;
	if ((sServerSock = accept (serverSock, (struct sockaddr *)&client, &client_len)) == -1)
	{
		fprintf(stderr, "Can't accept client\n");
		return -1;
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
-- INTERFACE: void SelectServer::send_msgs(int socket, char * data)
--					   int socket - server sock
--					   char * data - data that the server will send back to the client
--
-- RETURNS:  void
--
-- NOTES: Send Messages function used by the select server.
----------------------------------------------------------------------------------------------------------------------*/
void SelectServer::send_msgs(int socket, char * data)
{
	send(socket, data, _buflen, 0);
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
-- INTERFACE: int SelectServer::recv_msgs(int socket, char * bp)
--					  int socket - server socket
--					  char * bp - data that the server will receive from the client
--
-- RETURNS:  Socket Descriptor
--
-- NOTES: Send Messages function used by the select server.
----------------------------------------------------------------------------------------------------------------------*/
int SelectServer::recv_msgs(int socket, char * bp)
{
	int n, bytes_to_read = _buflen;
	printf("recv %d\n", socket);
	while ((n = recv (socket, bp, bytes_to_read, 0)) < bytes_to_read)
	{

		if(n == -1){
			
			printf("errno %d on socket %d\n",errno, socket);
			ClientData::Instance()->removeClient(socket);
			close(socket);
			FD_CLR(socket, &allset);
			for (int i =0; i< maxi; ++i){
				if(client[i]==socket){
					client[i] = -1;
					break;
				}
			}
			return -1;
		} else if (n == 0){
			printf("socket was gracefully closed by other side %d\n",socket);
			ClientData::Instance()->removeClient(socket);
			close(socket);
			FD_CLR(socket, &allset);
			for (int i =0; i< maxi; ++i){
				if(client[i]==socket){
					client[i] = -1;
					break;
				}
			}
			
			return -1;
		}
		//printf("bytes read and to read %d %d /n", n, bytes_to_read);
		bp += n;
		bytes_to_read -= n;
	}
	//printf("end recv %d\n",socket);
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
-- INTERFACE: int SelectServer::set_sock_option(int listenSocket)
--						int listenSocket - listening socket
--
-- RETURNS:  N/A
--
-- NOTES: Function that sets the listening socket options.
----------------------------------------------------------------------------------------------------------------------*/
int SelectServer::set_sock_option(int listenSocket)
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
-- INTERFACE: void * SelectServer::process_client(void * args)
--
-- RETURNS:  0 on success
--
-- NOTES: Thread that processes the client by receiving and sending packets from the server. 
----------------------------------------------------------------------------------------------------------------------*/
void * SelectServer::process_client(void * args)
{	
	int sock;
	char buf[BUFLEN];
	SelectServer* mServer = SelectServer::Instance();
	
	while(1){

		mServer->fd_queue.pop(sock, mServer->timeout);

		printf("fd socket from queue%d\n", sock);
		if(!ClientData::Instance()->has(sock)){
			continue;
		}
		if(mServer->recv_msgs(sock, buf)<0){
			continue;
		}
		ClientData::Instance()->setRtt(sock);
		//printf("Received: %s\n", buf);	
		mServer->send_msgs(sock, buf);	
		ClientData::Instance()->recordData(sock, BUFLEN);
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
-- INTERFACE: int SelectServer::set_port(int port)
--					 int port - server port specified
--
-- RETURNS:  N/A
--
-- NOTES: Sets server port when starting the server.
----------------------------------------------------------------------------------------------------------------------*/
int SelectServer::set_port(int port){
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
-- INTERFACE: int SelectServer::set_num_threads(int num)
--					int num - number of worker threads server should use
--
-- RETURNS:  N/A
--
-- NOTES: Sets server port when starting the server.
----------------------------------------------------------------------------------------------------------------------*/
int SelectServer::set_num_threads(int num){
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
-- INTERFACE: int SelectServer::setBufLen(int buflen)
--					int buflen - buffer length
--
-- RETURNS:  N/A
--
-- NOTES: sets buffer length for send and recv functions
----------------------------------------------------------------------------------------------------------------------*/
int SelectServer::setBufLen(int buflen){
	_buflen = buflen;
	return 1;
}

