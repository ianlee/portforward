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
	portIterator = forward_list.begin();
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
void Config::printSocketList()
{
	for(std::map<int, DestData>::iterator it = socketDesc_list.begin(); it != socketDesc_list.end(); ++it)
	{
		std::cout << "Socket Desc ID: " << it->first << " Server Dest Host: " << it->second.destAddr;
		std::cout << " Server Dest Port: " << it->second.destPort << std::endl;
	}
}
int Config::setFilename(const char * filename)
{
	fp = fopen(filename,"r+");
	return (fp == NULL) ? -1 : 0;
}
int Config::storeSocketIntoMap(const int port, const int socket)
{
	std::map<int, DestData>::iterator it = forward_list.find(port);

	if(it != forward_list.end())
	{
		socketDesc_list.insert(std::pair<int, DestData>(socket, it->second));
		return 0;
	}
	else
	{
		std::cerr << "Can't find port " << port << " for inserting socket desc " << socket << std::endl;
		return -1;
	}

}


int Config::getPort(){
	int rtn = 0;
	if(portIterator != forward_list.end()){

	    rtn = portIterator->first;
	    ++portIterator;
	}
	return rtn;
}
int Config::getData(const int socket, DestData* data){
	std::map<int, DestData>::iterator it = socketDesc_list.find(socket);

	if(it != socketDesc_list.end())
	{
	    for(int i = 0; i< BUFLEN; ++i){
    	    data->destAddr[i]=it->second.destAddr[i];
	    }
        data->destPort=it->second.destPort;
		return  1;
	}
	else
	{
		std::cerr << "Can't find socket desc " << socket << std::endl;
		return 0;
	}
}
