#ifndef __REQUEST_H__
#define __REQUEST_H__

#include <netinet/in.h>
#include <string>

#define BUFSIZE	1023



/**
 * Directory with files served by webserver
 */
extern std::string documentRoot;



/**
 * Structure with request info
 */
typedef struct requestInfo {
	int connected;
	sockaddr_in *client_addr;
	socklen_t *sin_size;
} reqInfo;

extern bool showDebug;

void parseRequest(int);

int acceptAndLoadBuffer(reqInfo, std::string *);

void * processHttpRequest(void *);

#endif