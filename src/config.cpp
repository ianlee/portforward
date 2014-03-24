#include "config.h"
/*-------------------------------------------------------------------------------------------------------------------- 
-- FUNCTION: parseFile()
--
-- DATE: 2014/02/21
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ian Lee, Luke Tao
--
-- PROGRAMMER: Ian Lee, Luke Tao
--
-- INTERFACE: int Config::parseFile()
--	
--						
--
-- RETURNS:  0
--
-- NOTES: Parses config file to create map of the config data for future use.
----------------------------------------------------------------------------------------------------------------------*/
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
/*-------------------------------------------------------------------------------------------------------------------- 
-- FUNCTION: printForwardList()
--
-- DATE: 2014/02/21
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ian Lee, Luke Tao
--
-- PROGRAMMER: Ian Lee, Luke Tao
--
-- INTERFACE: void Config::printForwardList()
--	
--						
--
-- RETURNS:  0
--
-- NOTES: Prints config data for testing purposes
----------------------------------------------------------------------------------------------------------------------*/
void Config::printForwardList()
{
	for(std::map<int, DestData>::iterator it = forward_list.begin(); it != forward_list.end(); ++it)
	{
		std::cout << "Client Port: " << it->first << " Server Dest Host: " << it->second.destAddr;
		std::cout << " Server Dest Port: " << it->second.destPort << std::endl;
	}
}
/*-------------------------------------------------------------------------------------------------------------------- 
-- FUNCTION: printSocketList()
--
-- DATE: 2014/02/21
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ian Lee, Luke Tao
--
-- PROGRAMMER: Ian Lee, Luke Tao
--
-- INTERFACE: int Config::printSocketList()
--	
--						
--
-- RETURNS:  0
--
-- NOTES: Prints config data for testing purposes
----------------------------------------------------------------------------------------------------------------------*/
void Config::printSocketList()
{
	for(std::map<int, DestData>::iterator it = socketDesc_list.begin(); it != socketDesc_list.end(); ++it)
	{
		std::cout << "Socket Desc ID: " << it->first << " Server Dest Host: " << it->second.destAddr;
		std::cout << " Server Dest Port: " << it->second.destPort << std::endl;
	}
}
/*-------------------------------------------------------------------------------------------------------------------- 
-- FUNCTION: setFilename
--
-- DATE: 2014/02/21
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ian Lee, Luke Tao
--
-- PROGRAMMER: Ian Lee, Luke Tao
--
-- INTERFACE: int Config::setFilename(const char * filename)
--					const char * filename - filename of config file.
--						
--
-- RETURNS:  0
--
-- NOTES: Sets the filename of the config file.
----------------------------------------------------------------------------------------------------------------------*/
int Config::setFilename(const char * filename)
{
	fp = fopen(filename,"r+");
	return (fp == NULL) ? -1 : 0;
}
/*-------------------------------------------------------------------------------------------------------------------- 
-- FUNCTION: storeSocketIntoMap
--
-- DATE: 2014/02/21
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ian Lee, Luke Tao
--
-- PROGRAMMER: Ian Lee, Luke Tao
--
-- INTERFACE: int Config::storeSocketIntoMap(const int port, const int socket)
--						const int port - port that was stored and used to make socket.
--						const int socket - corresponding socket desc to be stored
--						
--
-- RETURNS:  0
--
-- NOTES: Stores data of config file where key is socket descriptor instead of the port.
----------------------------------------------------------------------------------------------------------------------*/
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

/*-------------------------------------------------------------------------------------------------------------------- 
-- FUNCTION: getPort()
--
-- DATE: 2014/02/21
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ian Lee, Luke Tao
--
-- PROGRAMMER: Ian Lee, Luke Tao
--
-- INTERFACE: int Config::getPort()
--	
--						
--
-- RETURNS:  next port number else 0
--
-- NOTES: returns next port number to be used (iterates through map)
----------------------------------------------------------------------------------------------------------------------*/
int Config::getPort(){
	int rtn = 0;
	if(portIterator != forward_list.end()){

	    rtn = portIterator->first;
	    ++portIterator;
	}
	return rtn;
}

/*-------------------------------------------------------------------------------------------------------------------- 
-- FUNCTION: getData
--
-- DATE: 2014/02/21
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ian Lee, Luke Tao
--
-- PROGRAMMER: Ian Lee, Luke Tao
--
-- INTERFACE: int Config::getData(const int socket, DestData* data){
--					const int socket - key for data
--					DestData* data - pointer where data will be returned.
--						
--
-- RETURNS:  1 if successful else 0
--
-- NOTES: gives data struct into data.
----------------------------------------------------------------------------------------------------------------------*/
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
