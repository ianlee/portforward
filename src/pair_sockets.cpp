#include "pair_sockets.h"
/*-------------------------------------------------------------------------------------------------------------------- 
-- FUNCTION: insertPairOfSockets
--
-- DATE: 2014/02/21
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ian Lee, Luke Tao
--
-- PROGRAMMER: Ian Lee, Luke Tao
--
-- INTERFACE: void PairSockets::insertPairOfSockets(const int s1, const int s2)
--						const int s1 - socket 1
--						const int s2 - socket 2
--
-- RETURNS:  N/A
--
-- NOTES: Adds pair of socket descriptors to a map as 2 entries.  This is so that finding either socket will be constant speed lookup.
----------------------------------------------------------------------------------------------------------------------*/
void PairSockets::insertPairOfSockets(const int s1, const int s2)
{
	socket_list.insert(std::pair<int, int>(s1, s2));
	socket_list.insert(std::pair<int, int>(s2, s1));
}
/*-------------------------------------------------------------------------------------------------------------------- 
-- FUNCTION: getSocketFromList
--
-- DATE: 2014/02/21
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ian Lee, Luke Tao
--
-- PROGRAMMER: Ian Lee, Luke Tao
--
-- INTERFACE: void PairSockets::getSocketFromList(const int socket)
--						const int socket - key of pair of sockets to find

--
-- RETURNS:  N/A
--
-- NOTES: finds other socket from pair.
----------------------------------------------------------------------------------------------------------------------*/
int PairSockets::getSocketFromList(const int socket)
{
	_mutex.lock();
	std::map<int, int>::iterator it = socket_list.find(socket);
	_mutex.unlock();
	return (it != socket_list.end()) ? it->second : -1;
}
/*-------------------------------------------------------------------------------------------------------------------- 
-- FUNCTION: removeSocketFromList
--
-- DATE: 2014/02/21
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ian Lee, Luke Tao
--
-- PROGRAMMER: Ian Lee, Luke Tao
--
-- INTERFACE: void PairSockets::removeSocketFromList(const int keySocket)
--						const int keySocket - key of pair of sockets to remove
--						
--
-- RETURNS:  N/A
--
-- NOTES: removes pair of socket descriptors from a map
----------------------------------------------------------------------------------------------------------------------*/
int PairSockets::removeSocketFromList(const int keySocket)
{
	_mutex.lock();std::map<int, int>::iterator it = socket_list.find(keySocket);
	if(it != socket_list.end())
	{
		int destSocket = it->second;
		socket_list.erase(it->first);
		socket_list.erase(destSocket);
		_mutex.unlock();
		return 0;
	}
	else
	{
		std::cerr << "Cannot find and remove socket " << keySocket << std::endl;
		_mutex.unlock();
		return -1;
	}
}
