#include "epoll_server.h"

EpollServer::EpollServer(int port) : _port(port) {}

EpollServer* EpollServer::m_pInstance = NULL;
EpollServer* EpollServer::Instance()
{
	if (!m_pInstance)   // Only allow one instance of class to be generated.
		m_pInstance = new EpollServer(TCP_PORT);
	return m_pInstance;
}


int EpollServer::run()
{
	int socks [MAXCLIENTS];
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
	
	// Make the server listening socket non-blocking
    if (fcntl (serverSock, F_SETFL, O_NONBLOCK | fcntl (serverSock, F_GETFL, 0)) == -1) 
		fprintf(stderr,"fcntl");
		
	maxfd = serverSock;
	maxi = -1;
	
	epoll_fd = epoll_create(MAXCLIENTS);
    if (epoll_fd == -1) 
		fprintf(stderr,"epoll_create");
	// Add the server socket to the epoll event loop
	event.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLET;
	event.data.fd = serverSock;
	if (epoll_ctl (epoll_fd, EPOLL_CTL_ADD, serverSock, &event) == -1) 
		fprintf(stderr,"epoll_ctl");
	
	while(true){
		
		nready = epoll_wait (epoll_fd, events, MAXCLIENTS, -1);
		
		for (i = 0; i <= maxi; i++){	// check all clients for data
     		
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
				int sock = accept_client();				
				continue;
	    	}

	    		// Case 3: One of the sockets has read data
			fd_queue.push(events[i].data.fd, timeout);
	    	
			
			
			/*int sockfd;
			if ((sockfd = client[i]) < 0){
				continue;
			}
			if (FD_ISSET(sockfd, &rset)) {
				
         		if (--nready <= 0){
					break;        // no more readable descriptors
				}
			}*/
     	}
	
	}
	close(serverSock);
	return 0;
}

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

void EpollServer::listen_for_clients()
{
	// Listen for connections
	// queue up to 10000 connect requests
	listen(serverSock, 5);
}

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

void EpollServer::send_msgs(int socket, char * data)
{
	send(socket, data, BUFLEN, 0);
}

int EpollServer::recv_msgs(int socket, char * bp)
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
			close(socket);
			for (int i =0; i< maxi; ++i){
				//if(client[i]==socket){
					//client[i] = -1;
				//}
			}
			
			break;
		}
	}
printf("end recv %d\n", socket);
	return socket;
}

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
void * EpollServer::process_client(void * args)
{	
	int sock;
	char buf[BUFLEN];
	EpollServer* mServer = EpollServer::Instance();
	
	while(1){
		mServer->fd_queue.pop(sock, mServer->timeout);
		mServer->recv_msgs(sock, buf);
		printf("Received: %s\n", buf);	
		mServer->send_msgs(sock, buf);	
	}		            				
	
	return (void*)0;

}
int EpollServer::set_port(int port){
	_port = port;
	return 1;
}


