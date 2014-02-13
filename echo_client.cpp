#include "echo_client.h"

Client::Client(char * host, int port, int t_sent) : _host(host), _port(port), times_sent(t_sent) {}

int Client::run()
{	

	//Create multiple processes and each process will be a single client essentially

	//sprintf(sendbuf, "foobar");
	for(int i = 0; i < 5; i++)
	{
		switch(fork())
		{
			case -1:
				std::cerr << "An error has occurred" << std::endl;
				break;
			case 0:
				child_client_process(i);
				break;
			default:
				break;
		}
	}
	wait_for_client_processes();

	std::cout << "All clients finished processing" << std::endl;
	return 0;
}
int Client::child_client_process(int client_num)
{
	std::cout << "Processing client " << client_num << std::endl;
	//printf("Sending: %s\n", sendbuf);
	//send_msgs(clientSock, sendbuf);

	//recv_msgs(clientSock, recvbuf);
	//printf("Receiving: %s\n", recvbuf);
	fflush(stdout);
	exit(0);
	return 0;
}

void Client::wait_for_client_processes()
{
	while (wait(NULL) > 0) {
   		if (errno == ECHILD) { /* If there are no more child processes running */
      		break;
   		}
	}
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
