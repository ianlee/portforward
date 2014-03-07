#include "pair_sockets.h"

void PairSockets::insertPairOfSockets(const int s1, const int s2)
{
	socket_list.insert(std::pair<int, int>(s1, s2));
	socket_list.insert(std::pair<int, int>(s2, s1));
}

int PairSockets::getSocketFromList(const int socket)
{
	std::map<int, int>::iterator it = socket_list.find(socket);
	return (it != socket_list.end()) ? it->first : -1;
}

int PairSockets::removeSocketFromList(const int keySocket)
{
	std::map<int, int>::iterator it = socket_list.find(keySocket);
	if(it != socket_list.end())
	{
		int destSocket = it->second;
		socket_list.erase(it->first);
		socket_list.erase(destSocket);
		return 0;
	}
	else
	{
		std::cerr << "Cannot find and remove socket " << keySocket << std::endl;
		return -1;
	}
}
