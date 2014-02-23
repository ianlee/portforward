#ifndef CLIENT_DATA_H
#define CLIENT_DATA_H

#include <iostream>
#include <vector>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <string.h>
#include <map>
#include <mutex>
#include <sys/time.h>

#define BUFLEN 255
struct client_data {

	char client_addr[BUFLEN];
	int client_port;
	int socket;
	struct timeval last_time;
	int rtt;
};



class ClientData {

public:
	static ClientData* Instance();
	~ClientData();
	int print();
	int addClient(int socket, char* client_addr, int client_port);
	int removeClient(int socket);
	int setFile(const char* filename);
 	int empty();
	int has(int sock);
	int setRtt(int sock);
private:
	FILE* _file;
	std::map<int, client_data> list_of_clients;
	static ClientData* m_pInstance;
	std::mutex _mutex;

};





#endif
