#include "select_server.h"

SelectServer::SelectServer(int port) : _port(port) {}

SelectServer* SelectServer::m_pInstance = NULL;
SelectServer* SelectServer::Instance()
{
	if (!m_pInstance)   // Only allow one instance of class to be generated.
		m_pInstance = new SelectServer(TCP_PORT);
	return m_pInstance;
}


int SelectServer::run()
{
	//int socks [MAXCLIENTS];
	pthread_t tids[MAXCLIENTS];
	int i;
	
	
	for(int i = 0; i < NUMTHREADS; i++)
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

void SelectServer::listen_for_clients()
{
	// Listen for connections
	// queue up to 10000 connect requests
	listen(serverSock, 5);
}

int SelectServer::accept_client()
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
	printf(" Remote Address:  %s\n", inet_ntoa(client.sin_addr));
	return sServerSock;
}

void SelectServer::send_msgs(int socket, char * data)
{
	send(socket, data, BUFLEN, 0);
}

int SelectServer::recv_msgs(int socket, char * bp)
{
	int n, bytes_to_read = BUFLEN;
	printf("recv %d\n", socket);
	while ((n = recv (socket, bp, bytes_to_read, 0)) < bytes_to_read)
	{

		if(n == -1){
			printf("error %d %d %d\n", bytes_to_read, n, socket);
			printf("errno %d\n",errno);
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
		printf("bytes read and to read %d %d /n", n, bytes_to_read);
		bp += n;
		bytes_to_read -= n;
	}
	printf("end recv %d\n",socket);
	return 0;
}

int SelectServer::set_sock_option(int listenSocket)
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
		//printf("Received: %s\n", buf);	
		mServer->send_msgs(sock, buf);	
	}		            				
	
	return (void*)0;

}
int SelectServer::set_port(int port){
	_port = port;
	return 1;
}


