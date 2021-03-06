#ifndef __TCPHELPER_H__
#define __TCPHELPER_H__

#include <string>
#include "request.h"

class TCPHelper {

protected:
	std::string response;
	std::string request;
	std::string host;

	int socketNr;
	int port;

public:	
	TCPHelper();
	~TCPHelper();



	void setHost(std::string);
	void setPort(int);
	std::string getResponse();
	std::string getRequest();
	
	
	void connect();
	int callSocket();
	void setSocket(int);
	int getSocket();
	int startServer();
	int write(std::string);
	int read();
};


#endif