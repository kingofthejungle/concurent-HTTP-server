
#include <dispatch/dispatch.h>

#include "request.h"
#include "common.h"
#include <sys/time.h>
#include <time.h>



void printError(std::string err) {
	fprintf(stderr, "ERROR: %s\n", err.c_str());
}



void acceptRequest(int sock, reqInfo * req){

	// accept connection
	req->connected = accept(sock, (struct sockaddr *) &req->client_addr, (socklen_t *) &req->sin_size);
	if (req->connected == -1) {
		printError("Problem s prijetim spojeni");
	}
}



void serverMainLoop(int sock, void * function) {
	void (*parse_request)(reqInfo) = (void (*)(reqInfo))function;

	timeval time;
//	gettimeofday(&time, NULL);
//	double microtime = time.tv_sec+(time.tv_usec/1000000.0);
	double start, end;
	
	while(1) {
		gettimeofday(&time, NULL);
		start = time.tv_sec+(time.tv_usec/1000000.0);
		reqInfo request;
		acceptRequest(sock, &request);

		if(showDebug) {
			printf("serverMainLoop - reqInfo: %i\n", request.connected);
		}

		parse_request(request);
		
		gettimeofday(&time, NULL);
		end = time.tv_sec+(time.tv_usec/1000000.0);
		//printf("interval: %f\n          %f\n          %f\n", start, end, end-start);
		if(showDebug)
		printf("delta: %f\n", end-start);
	}
}



void serverMainSources(int sock, void * function) {
	void (*parse_request)(reqInfo) = (void (*)(reqInfo))function;	
	
	// creating dispatch source and geting the queue
	dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
	dispatch_source_t readSource = dispatch_source_create(DISPATCH_SOURCE_TYPE_READ, sock, 0, queue);

	if (!readSource) {
		printError("Unable to create source!");
		close(sock);
		return;
	}
	
	
	dispatch_source_set_cancel_handler(readSource, ^{
		close(sock); 
	});
	
	
	dispatch_source_set_event_handler(readSource, ^{
		reqInfo request;
		acceptRequest(sock, &request);
		parse_request(request);
	});
	
	// Call the dispatch_resume function to start processing events
    dispatch_resume(readSource);
	dispatch_main();	
}