#include "multi_thread_server.h"

MultiThreadServer::MultiThreadServer(int port) : _port(port) {}

int MultiThreadServer::run()
{
	char buf[BUFLEN];
	serverSock = create_socket();
	serverSock = bind_socket();
	listen_for_clients();
	
	accept_client();
	recv_msgs(newServerSock, buf);
	printf("Received: %s\n", buf);	

	printf("Sending: %s\n", buf);
	send_msgs(newServerSock, buf);	

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
	// queue up to 5 connect requests
	listen(serverSock, 5);
}

int MultiThreadServer::accept_client()
{
	struct	sockaddr_in client;
	unsigned int client_len = sizeof(client);
	
	if ((newServerSock = accept (serverSock, (struct sockaddr *)&client, &client_len)) == -1)
	{
		fprintf(stderr, "Can't accept client\n");
		exit(1);
	}
	printf(" Remote Address:  %s\n", inet_ntoa(client.sin_addr));
	return newServerSock;
}

void MultiThreadServer::send_msgs(int socket, char * data)
{
	send(socket, data, BUFLEN, 0);
}

int MultiThreadServer::recv_msgs(int socket, char * bp)
{
	int n, bytes_to_read = BUFLEN;
	while ((n = recv (socket, bp, bytes_to_read, 0)) < BUFLEN)
	{
		bp += n;
		bytes_to_read -= n;
	}
	return newServerSock;
}

void * process_client(void *)
{
	return 0;
}
