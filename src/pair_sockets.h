#ifndef PAIR_SOCKETS_H
#define PAIR_SOCKETS_H

#include <iostream>
#include <map>
#include <mutex>

class PairSockets
{
	public:
		void insertPairOfSockets(const int s1, const int s2);
		int getSocketFromList(const int socket);
		int removeSocketFromList(const int keySocket);
	private:
		std::map<int, int> socket_list;
		std::mutex _mutex;
};

#endif
