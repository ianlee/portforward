#include "echo_client.h"

Client::Client(char * host, int port) : _host(host), _port(port) {}
int Client::run()
{	
	char sendbuf[BUFLEN];	
	clientSock = create_socket();
	clientSock = connect_to_server(clientSock, _host);

	while(fgets(sendbuf, BUFLEN, stdin))
	{
		send_msgs(clientSock, sendbuf);
		fflush(stdout);
	}
	return 0;
}
int Client::create_socket()
{
	int sd;

	// Create the socket
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("Cannot create socket");
	}
	return sd;
	
}
int Client::connect_to_server(int socket, char * host)
{
	struct sockaddr_in server;
	struct hostent	*hostptr;
	char **pptr;
	char str[16];

	bzero((char *)&server, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(_port);
	if ((hostptr = gethostbyname(host)) == NULL)
	{
		fprintf(stderr, "Unknown server address\n");
		exit(1);
	}
	bcopy(hostptr->h_addr, (char *)&server.sin_addr, hostptr->h_length);

	if (connect (socket, (struct sockaddr *)&server, sizeof(server)) == -1)
	{
		fprintf(stderr, "Can't connect to server\n");
		perror("connect");
		return 0;
	}
	printf("Connected:    Server Name: %s\n", hostptr->h_name);
	pptr = hostptr->h_addr_list;
	printf("\t\tIP Address: %s\n", inet_ntop(hostptr->h_addrtype, *pptr, str, sizeof(str)));
	return socket;
}

int Client::send_msgs(int socket, char * data)
{
	return send(socket, data, BUFLEN, 0);
}
int Client::recv_msgs(int socket, char * buf)
{
	int bytes_read = 0, total_bytes_read = 0;
	int bytes_to_read = BUFLEN;

	while ((bytes_read = recv (socket, buf, bytes_to_read, 0)) < BUFLEN)
	{
		buf += bytes_read;
		bytes_to_read -= bytes_read;
		total_bytes_read += bytes_read;
	}

	return total_bytes_read;
}