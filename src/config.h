#ifndef CONFIG_H
#define CONFIG_H

#include <iostream>
#include <map>
#include <cstdio>
#include <cstring>

#define BUFLEN 255

struct DestData
{
	char destAddr[BUFLEN];
	int destPort;
};

class Config
{
	public:
		int setFilename(const char * filename);
		void printForwardList();
		int parseFile();
	private:
		FILE *fp;
		int socketDesc, clntPort, destPort;
		char destAddr[BUFLEN];
		std::map<int, DestData> forward_list;
		std::map<int, DestData> socketDesc_list;
		
};

#endif
