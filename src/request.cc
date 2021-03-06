/*
 *  request.c
 *  obsahuje vse potrebne pro zpracovani HTTP pozadavku
 *
 *  Created by Michal Svec on 8.3.11.
 */

#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "request.h"
#include "common.h"

#include "HttpHelper.h"

#include "gcd.h"
#include "pthreads.h"	
#include "fork.h"


using namespace std;





/**
 * Nacteni souboru ze slozky webserveru 
 * @param name of a file
 * @param buffer for content
 */
bool loadFile(string filePath, string &content) {

	string line;
	ifstream file (filePath.c_str());

//  cwd detecting	
//	char path1[1000];
//	getcwd(path1, 1000);
	
	if(file) {
		while(getline(file,line)) {
			content += line;
			content += "\n";
		}
		return true;
	} 
	else	// file does not exists
		return false;
}



void getRequestInfo(void * req, reqInfo * data) {
	data->socket = ((reqInfo *) req)->socket;
	data->useAVG  = ((reqInfo *) req)->useAVG;
	data->avgHost = ((reqInfo *) req)->avgHost;
	data->avgPort = ((reqInfo *) req)->avgPort;
}



/**
 * Main method for HTTP request processing
 *
 * @param void * reqInfo structure wotj request information - (void *) because pthreads needs this type as a parameter 
 */
void * processHttpRequest(void * req) {

	reqInfo data;
	int bytesRecvd;
	// String - document to read from webserver public folder
	string file = "";

	// Need local copy because of problem with pthreads - which tooks pointer 
	getRequestInfo(req, &data);

	AVGHelper* http = new AVGHelper();
	if(data.useAVG) {
		http->useAVG = true;
	}

	http->setSocket(data.socket);
	
	string buffer;
	string fileContent;
	string errMsg;
	
	bytesRecvd = http->read();
	if (bytesRecvd < 0) {
		fprintf(stderr,("read() error\n"));
		return NULL;
	}

	if(http->getResponse().length() < 1) {
		cerr << "Empty buffer - terminating" << endl;
		return NULL;
	}

	http->parseHttpRequest(http->getResponse(), &file);
	if(file == "") {
		printError("Wrong offset - can't parse file name.");
		return NULL;
	}

	
	HTTPHelper::HTTPStatus status = http->getFile(file, fileContent);
	
	switch (status) {
		case HTTPHelper::HTTP_INFECTED:
			errMsg = "403 ";
			break;
		case HTTPHelper::HTTP_NOTFOUND:
			errMsg = "404 ";
			break;
		case HTTPHelper::HTTP_OK:
		default:
			errMsg = "200 ";
			break;
	}
	errMsg += file;
	dispatchPrint(errMsg);

	
	if(isDispatchSuitable())	
		dispatchIncreaseResponded();

	http->buildResponse(status, file, fileContent);	
	try {
		http->write(http->getResponse());
	} catch (char * e) {
		printError(e);
	}
	

	delete http; http = NULL;

	return NULL;
}



