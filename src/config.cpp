#include "config.h"

int Config::parseFile()
{
	struct DestData temp;

	while(fscanf(fp, "%d %s %d", &clntPort, destAddr, &destPort) == 3)
	{
		strcpy(temp.destAddr, destAddr);
		temp.destPort = destPort;
		forward_list.insert(std::pair<int, DestData>(clntPort, temp));
	}
	fclose(fp);
	return 0;
}

void Config::printForwardList()
{
	for(std::map<int, DestData>::iterator it = forward_list.begin(); it != forward_list.end(); ++it)
	{
		std::cout << "Client Port: " << it->first << " Server Dest Host: " << it->second.destAddr;
		std::cout << " Server Dest Port: " << it->second.destPort << std::endl;
	}
}
int Config::setFilename(const char * filename)
{
	fp = fopen(filename,"r+");
	return (fp == NULL) ? -1 : 0;
}

int main()
{
	Config config;
	config.setFilename("test.txt");
	config.parseFile();
	config.printForwardList();
	return 0;
}
