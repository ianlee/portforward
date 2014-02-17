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
	_file = fopen(filename,"a");
	return 0;
}

int ClientData::print(){
	fprintf(_file,"clients: %lu", list_of_clients.size() );
	return 0;
}
int ClientData::addClient(int socket, char* client_addr, int client_port){
	struct client_data tempData;
	tempData.socket=socket;
	memcpy(tempData.client_addr,client_addr, strlen( client_addr));
	tempData.client_port = client_port;
	list_of_clients.insert(std::pair<int, client_data>(socket, tempData));
	return 0;
}
int ClientData::removeClient(int socket){
	list_of_clients.erase(socket);
	return 0;
}
int ClientData::empty(){
	return list_of_clients.empty();
}



