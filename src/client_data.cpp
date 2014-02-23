#include "client_data.h"

/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: client_data.cpp - Hold the code for the client data class used by the scalable servers. 
--
-- PROGRAM: server
--
-- FUNCTIONS: ClientData* ClientData::Instance()
--			  ClientData::~ClientData()
--			  int ClientData::setFile(char* filename)
--			  int ClientData::print()
--			  int ClientData::addClient(int socket, char* client_addr, int client_port)
--			  int ClientData::removeClient(int socket)
--			  int ClientData::empty()
--			  int ClientData::has(int sock)
--            int ClientData::setRtt(int sock)
--
-- DATE: 2014/02/21
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ian Lee, Luke Tao
--
-- PROGRAMMER: Ian Lee, Luke Tao
--
-- NOTES: Used by other server type classes, this class handles the list of clients in an organized fashion.
----------------------------------------------------------------------------------------------------------------------*/

ClientData* ClientData::m_pInstance = NULL;
/*-------------------------------------------------------------------------------------------------------------------- 
-- FUNCTION: Instance
--
-- DATE: 2014/02/21
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ian Lee, Luke Tao
--
-- PROGRAMMER: Ian Lee, Luke Tao
--
-- INTERFACE: ClientData* ClientData::Instance()
--
-- RETURNS:  Returns the instance of class generated.
--
-- NOTES: Creates an instance of client data to be portably used by other classes.
----------------------------------------------------------------------------------------------------------------------*/
ClientData* ClientData::Instance()
{
	if (!m_pInstance)   // Only allow one instance of class to be generated.
		m_pInstance = new ClientData;
	return m_pInstance;
}
/*-------------------------------------------------------------------------------------------------------------------- 
-- FUNCTION: ~ClientData
--
-- DATE: 2014/02/21
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ian Lee, Luke Tao
--
-- PROGRAMMER: Ian Lee, Luke Tao
--
-- INTERFACE: ClientData::~ClientData()
--
-- RETURNS:  N/A
--
-- NOTES: A class destructor that closes the file pointer, iterates through the client list 
-- 		  and close each client socket.  
----------------------------------------------------------------------------------------------------------------------*/
ClientData::~ClientData(){
	fclose(_file);
	for( std::map<int, client_data>::iterator ii=list_of_clients.begin(); ii!=list_of_clients.end(); ++ii) {
		close( (*ii).first );
	}
}
/*-------------------------------------------------------------------------------------------------------------------- 
-- FUNCTION: setFile
--
-- DATE: 2014/02/21
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ian Lee, Luke Tao
--
-- PROGRAMMER: Ian Lee, Luke Tao
--
-- INTERFACE: int ClientData::setFile(const char* filename)
--				      const char* filename - file name specified
--
-- RETURNS:  0 on success
--
-- NOTES: This function opens a file under the specified filename and returns 0.
----------------------------------------------------------------------------------------------------------------------*/
int ClientData::setFile(const char* filename){
	_file = fopen(filename,"a+");
	if(_file==NULL) return -1;
	return 0;
}
/*-------------------------------------------------------------------------------------------------------------------- 
-- FUNCTION: print
--
-- DATE: 2014/02/21
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ian Lee, Luke Tao
--
-- PROGRAMMER: Ian Lee, Luke Tao
--
-- INTERFACE: int ClientData::print()
--
-- RETURNS:  Number of clients in the list.
--
-- NOTES: Print number of clients and the avg RTT of clients in the specified file pointer.
----------------------------------------------------------------------------------------------------------------------*/
int ClientData::print(){
	unsigned long size;
	double avgRtt;
	double totalRtt;
	
	_mutex.lock();
	size =  list_of_clients.size() ;
	
	for(std::map<int,client_data>::iterator it = list_of_clients.begin(); it!=list_of_clients.end(); ++it){
		totalRtt += it->second.rtt;
	}
	_mutex.unlock();
	avgRtt = totalRtt / size;
	fprintf(_file,"clients: %lu \tRTT: %lf\n", size, avgRtt);
	fflush(_file);
	return list_of_clients.size();
}
/*-------------------------------------------------------------------------------------------------------------------- 
-- FUNCTION: addClient
--
-- DATE: 2014/02/21
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ian Lee, Luke Tao
--
-- PROGRAMMER: Ian Lee, Luke Tao
--
-- INTERFACE: int ClientData::addClient(int socket, char* client_addr, int client_port)
--					int socket        - client socket
--					char* client_addr - client address struct
--					int client_port   - client port
--
-- RETURNS:  0 on sucess
--
-- NOTES: Adds a client to the map container list.
----------------------------------------------------------------------------------------------------------------------*/
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
/*-------------------------------------------------------------------------------------------------------------------- 
-- FUNCTION: removeClient
--
-- DATE: 2014/02/21
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ian Lee, Luke Tao
--
-- PROGRAMMER: Ian Lee, Luke Tao
--
-- INTERFACE: int ClientData::removeClient(int socket)
--
-- RETURNS:  0 on success
--
-- NOTES: Erases the client data from the list based on the client socket passed in.
----------------------------------------------------------------------------------------------------------------------*/
int ClientData::removeClient(int socket){
	_mutex.lock();
	list_of_clients.erase(socket);
	_mutex.unlock();
	return 0;
}
/*-------------------------------------------------------------------------------------------------------------------- 
-- FUNCTION: empty
--
-- DATE: 2014/02/21
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ian Lee, Luke Tao
--
-- PROGRAMMER: Ian Lee, Luke Tao
--
-- INTERFACE: int ClientData::empty()
--
-- RETURNS:  true if the number of active clients are empty, false otherwise.
--
-- NOTES: Returns true or false if the number of clients in the list are empty.
----------------------------------------------------------------------------------------------------------------------*/
int ClientData::empty(){
	int empty;
	_mutex.lock();
	empty = list_of_clients.empty();
	_mutex.unlock();
	return empty;

}
/*-------------------------------------------------------------------------------------------------------------------- 
-- FUNCTION: has
--
-- DATE: 2014/02/21
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ian Lee, Luke Tao
--
-- PROGRAMMER: Ian Lee, Luke Tao
--
-- INTERFACE: int ClientData::has(int sock)
--			  	  int sock - client socket passed in
--
-- RETURNS:  true if the map contains the client data whose socket is matched to.
--
-- NOTES: Returns true or false if the map container has the client data with the matching client socket.
----------------------------------------------------------------------------------------------------------------------*/
int ClientData::has(int sock){
	_mutex.lock();
	int rtn = list_of_clients.count(sock);
	_mutex.unlock();
	return rtn;
}
/*-------------------------------------------------------------------------------------------------------------------- 
-- FUNCTION: setRtt
--
-- DATE: 2014/02/21
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ian Lee, Luke Tao
--
-- PROGRAMMER: Ian Lee, Luke Tao
--
-- INTERFACE: int ClientData::setRtt(int socket)
--                            int socket - socket for which rtt should be calculated for.
--
-- RETURNS:  calculated ReturnTripTime in milliseconds.  if it has no previous time value, returns -1
--
-- NOTES: calculates the RTT if previous timeval last_time exists.  Sets last_time to current time
----------------------------------------------------------------------------------------------------------------------*/
int ClientData::setRtt(int socket){
	int rtt = -1;
	struct timeval currTime;
	_mutex.lock();
	std::map<int,client_data>::iterator data = list_of_clients.find(socket);
	_mutex.unlock();
	if(data != list_of_clients.end()){
		if(data->second.last_time.tv_sec==0){
			
			gettimeofday(&currTime,NULL);
			//calc rtt
			rtt = (currTime.tv_sec - data->second.last_time.tv_sec ) * 1000000 + (currTime.tv_usec - data->second.last_time.tv_usec);
			data->second.rtt = rtt;
			printf("RTT: %d, socket: %d",rtt, socket);
		}
		data->second.last_time = currTime;
		
	}
	
	return rtt;
}


