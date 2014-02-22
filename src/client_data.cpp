#include "client_data.h"


ClientData* ClientData::m_pInstance = NULL;
ClientData* ClientData::Instance()
{
	if (!m_pInstance)   // Only allow one instance of class to be generated.
		m_pInstance = new ClientData;
	return m_pInstance;
}
ClientData::~ClientData(){
	fclose(_file);
	for( std::map<int, client_data>::iterator ii=list_of_clients.begin(); ii!=list_of_clients.end(); ++ii) {
		close( (*ii).first );
	}
}

int ClientData::setFile(char* filename){
	_file = fopen(filename,"a+");
	return 0;
}

int ClientData::print(){
	unsigned long size;
	_mutex.lock();
	size =  list_of_clients.size() ;
	_mutex.unlock();
	fprintf(_file,"clients: %lu\n", size);
	//fflush(_file);
	return list_of_clients.size();
}
int ClientData::addClient(int socket, char* client_addr, int client_port){
	struct client_data tempData;
	tempData.socket=socket;
	memcpy(tempData.client_addr,client_addr, strlen( client_addr));
	tempData.client_port = client_port;
	_mutex.lock();
	list_of_clients.insert(std::pair<int, client_data>(socket, tempData));
	_mutex.unlock();
	return 0;
}
int ClientData::removeClient(int socket){
	_mutex.lock();
	list_of_clients.erase(socket);
	_mutex.unlock();
	return 0;
}
int ClientData::empty(){
	int empty;
	_mutex.lock();
	empty = list_of_clients.empty();
	_mutex.unlock();
	return empty;

}
int ClientData::has(int sock){
	_mutex.lock();
	int rtn = list_of_clients.count(sock);
	_mutex.unlock();
	return rtn;
}



