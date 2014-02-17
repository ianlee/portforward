#include "echo_client.h"

Client::Client(char * host, int port, int t_sent) : _host(host), _port(port), times_sent(t_sent) {}

int Client::run()
{	
	//Create multiple processes and each process will be a single client essentially

	for(int i = 0; i < 1000; i++)
	{
		switch(fork())
		{
			case -1:
				std::cerr << "An error has occurred" << std::endl;
				break;
			case 0:
				child_client_process(i, times_sent);
				return 0;
			default:
				break;
		}
	}
	wait_for_client_processes();

	std::cout << "All clients finished processing" << std::endl;
	return 0;
}
void Client::child_client_process(int client_num, int times_sent)
{
	std::cout << "Processing client " << client_num << std::endl;
	int clientSock = create_socket();
	char sendBuf[] = {"FOOBAR"}, recvBuf[BUFLEN];
	clientSock = connect_to_server(clientSock, _host);
	if(clientSock >0){
//send_msgs(clientSock, sendBuf);
	std::cout << "Sending " << send_msgs(clientSock, sendBuf) << " bytes " << client_num << std::endl;
	std::cout << "Received " << recv_msgs(clientSock, recvBuf) << " bytes " << client_num << std::endl;


//recv_msgs(clientSock, recvBuf);
	std::cout << "Closing client " << client_num << " socket" << std::endl;
	close(clientSock);
}
	//fflush(stdout);
	exit(0);
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
//	printf("Connected:    Server Name: %s\n", hostptr->h_name);
	pptr = hostptr->h_addr_list;
//	printf("\t\tIP Address: %s\n", inet_ntop(hostptr->h_addrtype, *pptr, str, sizeof(str)));
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

	while ((bytes_read = recv (socket, buf, bytes_to_read, 0)) < bytes_to_read)
	{
		if(bytes_read == -1){
			printf("error %d %d %d\n", bytes_to_read, bytes_read, socket);
			printf("error %d\n",errno);
			break;
		} else if (bytes_read == 0){
			break;
		}

		buf += bytes_read;
		bytes_to_read -= bytes_read;
		total_bytes_read += bytes_read;
	}
	return total_bytes_read;
}
