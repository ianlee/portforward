#include "pair_sockets.h"

void PairSockets::insertPairOfSockets(const int s1, const int s2)
{
	socket_list.insert(std::pair<int, int>(s1, s2));
	socket_list.insert(std::pair<int, int>(s2, s1));
}

int PairSockets::getSocketFromList(const int socket)
{
	_mutex.lock();
	std::map<int, int>::iterator it = socket_list.find(socket);
	_mutex.unlock();
	return (it != socket_list.end()) ? it->first : -1;
}

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
