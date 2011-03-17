/*
 *  request.h
 *  httpserver
 *
 *  Created by Michal Svec on 8.3.11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include <netinet/in.h>

#define BUFSIZE	1023

void parseRequest(int);

int acceptAndLoadBuffer(int, struct sockaddr_in *client_addr, socklen_t *sin_size, char buffer[]);

void parseHttpRequest(int, char []);