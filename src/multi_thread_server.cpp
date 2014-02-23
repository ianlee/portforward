#include "multi_thread_server.h"

/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: multi_thread_server.cpp - Hold the code for the multi-thread server used by the echo client. 
--
-- PROGRAM: server
--
-- FUNCTIONS: MultiThreadServer::MultiThreadServer(int port)
--			  MultiThreadServer* MultiThreadServer::Instance()
--			  int MultiThreadServer::run()
--			  int MultiThreadServer::create_socket()
--			  int MultiThreadServer::bind_socket()
--			  void MultiThreadServer::listen_for_clients()
--			  int MultiThreadServer::accept_client()
--			  void MultiThreadServer::send_msgs(int socket, char * data)
--			  int MultiThreadServer::recv_msgs(int socket, char * bp)
--			  int MultiThreadServer::set_sock_option(int listenSocket)
--			  void * MultiThreadServer::process_client(void * args)
--			  int MultiThreadServer::set_port(int port)
--			  int MultiThreadServer::setBufLen(int buflen)
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
-- NOTES: Multi-thread server class tested by the echo client.
----------------------------------------------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------------------------------------- 
-- FUNCTION: MultiThreadServer (constructor)
--
-- DATE: 2014/02/21
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ian Lee, Luke Tao
--
-- PROGRAMMER: Ian Lee, Luke Tao
--
-- INTERFACE: MultiThreadServer::MultiThreadServer(int port)
--						   int port - server port
--
-- RETURNS:  N/A
--
-- NOTES: Multi-Thread Server constructor that will initialize the server port.
----------------------------------------------------------------------------------------------------------------------*/
MultiThreadServer::MultiThreadServer(int port) : _port(port) {}

MultiThreadServer* MultiThreadServer::m_pInstance = NULL;

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
-- INTERFACE: MultiThreadServer* MultiThreadServer::Instance()
--
-- RETURNS:  Returns the instance of class generated.
--
-- NOTES: Creates an instance of multi-thread server.
----------------------------------------------------------------------------------------------------------------------*/
MultiThreadServer* MultiThreadServer::Instance()
{
	if (!m_pInstance)   // Only allow one instance of class to be generated.
		m_pInstance = new MultiThreadServer(TCP_PORT);
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
-- INTERFACE: int MultiThreadServer::run()
--
-- RETURNS:  0 on success
--
-- NOTES: Main multi-thread server function
----------------------------------------------------------------------------------------------------------------------*/
int MultiThreadServer::run()
{
	int socks [MAXCLIENTS];
	pthread_t tids[MAXCLIENTS];
	
	serverSock = create_socket();
	serverSock = bind_socket();
	serverSock = set_sock_option(serverSock);
	listen_for_clients();

	for(int i = 0; i < MAXCLIENTS; i++)
	{
		socks[i] = accept_client();
		pthread_create(&tids[i], NULL, process_client, (void*)&(socks[i]));
	}
	for(int i = 0; i < MAXCLIENTS; i++)
		pthread_join(tids[i], NULL);
	
	pthread_exit(0);
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
-- INTERFACE: int MultiThreadServer::create_socket()
--
-- RETURNS:  Socket Descriptor
--
-- NOTES: Creates a socket and returns the socket descriptor on successful creation.
----------------------------------------------------------------------------------------------------------------------*/
int MultiThreadServer::create_socket()
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
-- INTERFACE: int MultiThreadServer::bind_socket()
--
-- RETURNS:  Server Socket Descriptor
--
-- NOTES: Function that binds an address to the server socket and returns the server socket.
----------------------------------------------------------------------------------------------------------------------*/
int MultiThreadServer::bind_socket()
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
-- INTERFACE: void MultiThreadServer::listen_for_clients()
--
-- RETURNS:  void
--
-- NOTES: Sets the number of clients the server will handle requests to.
----------------------------------------------------------------------------------------------------------------------*/
void MultiThreadServer::listen_for_clients()
{
	// Listen for connections
	// queue up to 10000 connect requests
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
-- INTERFACE: int MultiThreadServer::accept_client()
--
-- RETURNS:  New Socket Descriptor
--
-- NOTES: Function that blocks until a client connection request comes in. It will add the client to the list.
----------------------------------------------------------------------------------------------------------------------*/
int MultiThreadServer::accept_client()
{
	
	struct	sockaddr_in client;
	unsigned int client_len = sizeof(client);
	int sServerSock;
	if ((sServerSock = accept (serverSock, (struct sockaddr *)&client, &client_len)) == -1)
	{
		fprintf(stderr, "Can't accept client\n");
		exit(1);
	}
	
	ClientData::Instance()->addClient(sServerSock, inet_ntoa(client.sin_addr),client.sin_port );
	//printf("size of client data: %d\n", ClientData::Instance()->print());
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
-- INTERFACE: void MultiThreadServer::send_msgs(int socket, char * data)
--						int socket - server sock
--						char * data - data that the server will send back to the client
--
-- RETURNS:  void
--
-- NOTES: Send Messages function used by the multi-thread server.
----------------------------------------------------------------------------------------------------------------------*/
void MultiThreadServer::send_msgs(int socket, char * data)
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
-- INTERFACE: int MultiThreadServer::recv_msgs(int socket, char * bp)
--					       int socket - server socket
--					       char * bp - data that the server will receive from the client
--
-- RETURNS:  Socket Descriptor
--
-- NOTES: Send Messages function used by the multi-thread server.
----------------------------------------------------------------------------------------------------------------------*/
int MultiThreadServer::recv_msgs(int socket, char * bp)
{
	int n, bytes_to_read = BUFLEN;
printf("recv %d\n", socket);
	while ((n = recv (socket, bp, bytes_to_read, 0)) < bytes_to_read)
	{
		printf("%d %d /n", n, bytes_to_read);
		bp += n;
		bytes_to_read -= n;
		if(n == -1){
			printf("error %d %d %d\n", bytes_to_read, n, socket);
			printf("error %d\n",errno);
			break;
		} else if (n == 0){
			printf("socket was gracefully closed by other side %d\n",socket);
			ClientData::Instance()->removeClient(socket);
			pthread_exit(NULL);
			break;
		}
	}
printf("end recv %d\n", socket);
	return socket;
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
-- INTERFACE: int MultiThreadServer::set_sock_option(int listenSocket)
--						     int listenSocket - listening socket
--
-- RETURNS:  N/A
--
-- NOTES: Function that sets the listening socket options.
----------------------------------------------------------------------------------------------------------------------*/
int MultiThreadServer::set_sock_option(int listenSocket)
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
-- INTERFACE: void * MultiThreadServer::process_client(void * args)
--
-- RETURNS:  0 on success
--
-- NOTES: Thread that processes the client by receiving and sending packets from the server. 
----------------------------------------------------------------------------------------------------------------------*/
void * MultiThreadServer::process_client(void * args)
{	
	int sock = *((int*) args);
	printf("socket created                %d\n", sock);
	char buf[BUFLEN];
	MultiThreadServer* mServer = MultiThreadServer::Instance();
	while (! ClientData::Instance()->empty()){
		mServer->recv_msgs(sock, buf);
		//printf("Received: %s\n", buf);	
		ClientData::Instance()->setRtt(sock);
		//printf("Sending: %s\n", buf);
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
-- INTERFACE: int MultiThreadServer::set_port(int port)
--					      int port - server port specified
--
-- RETURNS:  N/A
--
-- NOTES: Sets server port when starting the server.
----------------------------------------------------------------------------------------------------------------------*/
int MultiThreadServer::set_port(int port){
	_port = port;
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
-- INTERFACE: int MultiThreadServer::setBufLen(int buflen)
--					int buflen - buffer length
--
-- RETURNS:  N/A
--
-- NOTES: sets buffer length for send and recv functions
----------------------------------------------------------------------------------------------------------------------*/
int MultiThreadServer::setBufLen(int buflen){
	_buflen = buflen;
	return 1;
}

