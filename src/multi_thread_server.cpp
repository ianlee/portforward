#include "multi_thread_server.h"

MultiThreadServer::MultiThreadServer(int port) : _port(port) {}

MultiThreadServer* MultiThreadServer::m_pInstance = NULL;
MultiThreadServer* MultiThreadServer::Instance()
{
	if (!m_pInstance)   // Only allow one instance of class to be generated.
		m_pInstance = new MultiThreadServer(TCP_PORT);
	return m_pInstance;
}


int MultiThreadServer::run()
{
	pthread_t tids[MAXCLIENTS];
	
	serverSock = create_socket();
	serverSock = bind_socket();
	serverSock = set_sock_option(serverSock);
	listen_for_clients();

	for(int i = 0; i < MAXCLIENTS; i++)
	{
		int sock;
		sock = accept_client();
printf("socket created                %d\n", sock);
		pthread_create(&tids[i], NULL, process_client, &sock);
	}
	for(int i = 0; i < MAXCLIENTS; i++)
		pthread_join(tids[i], NULL);
	
	pthread_exit(0);
	
	return 0;
}

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

void MultiThreadServer::listen_for_clients()
{
	// Listen for connections
	// queue up to 10000 connect requests
	listen(serverSock, MAXCLIENTS);
}

int MultiThreadServer::accept_client()
{
	struct client_data* data=(client_data*)malloc(sizeof(client_data));
	struct	sockaddr_in client;
	unsigned int client_len = sizeof(client);
	int sServerSock;
	if ((sServerSock = accept (serverSock, (struct sockaddr *)&client, &client_len)) == -1)
	{
		fprintf(stderr, "Can't accept client\n");
		exit(1);
	}
	data->socket = sServerSock;
	strcpy(data->client_addr, inet_ntoa(client.sin_addr));
	list_of_clients.push_back(data);
	printf(" Remote Address:  %s\n", inet_ntoa(client.sin_addr));
	return sServerSock;
}

void MultiThreadServer::send_msgs(int socket, char * data)
{
	send(socket, data, BUFLEN, 0);
}

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
			printf("error or no data %d %d %d\n", bytes_to_read, n, socket);
			printf("error %d\n",errno);
			break;
		}
	}
printf("end recv %d\n", socket);
	return newServerSock;
}

int MultiThreadServer::set_sock_option(int listenSocket)
{
	// Reuse address set
	int value = 1;
	if (setsockopt (listenSocket, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value)) == -1)
		perror("setsockopt failed\n");
	
	// Set buffer length to send or receive to BUFLEN.
	value = BUFLEN;
	/*if (setsockopt (listenSocket, SOL_SOCKET, SO_SNDBUF, &value, sizeof(value)) == -1)
		perror("setsockopt failed\n");
	
	if (setsockopt (listenSocket, SOL_SOCKET, SO_RCVBUF, &value, sizeof(value)) == -1)
		perror("setsockopt failed\n");*/

	return listenSocket;

}
void * MultiThreadServer::process_client(void * args)
{	
	int sock = *((int*) args);
	
	char buf[BUFLEN];
	MultiThreadServer* mServer = MultiThreadServer::Instance();

	mServer->recv_msgs(sock, buf);
	printf("Received: %s\n", buf);	

	printf("Sending: %s\n", buf);
	mServer->send_msgs(sock, buf);	
	pthread_exit(NULL);
}
int MultiThreadServer::set_port(int port){
	_port = port;
	return 1;
}


