#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "errors.h"




int connect_to_remote(char* url, int* sockfd_result){

    int status;
    struct addrinfo hints;
    struct addrinfo *servinfo;
    struct addrinfo *served_element;
    int sockfd, result;


    memset(&hints, 0, sizeof hints); // make sure the struct is empty
    hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
    hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

    //get address info for the url. If it fails, error.
    result = getaddrinfo(url, "https", &hints, &servinfo);
    if(result != 0){
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(result));
        return GETADDRFAIL;
    }
    served_element = servinfo; 

    // getaddrinfo returns a linked list of addrinfo objects.
    // we need to iterate through them to try and find one that works.
    while(served_element != NULL){

        // try and allocate a socket
        sockfd = socket(served_element->ai_family, served_element->ai_socktype, served_element->ai_protocol);

        if(sockfd == -1){
            perror("Socket Allocation Failed."); // cool c stdlib function that prints to stderror
            served_element = served_element->ai_next;
            continue;
        }

        // try to connect 
        result = connect(sockfd, served_element->ai_addr, served_element->ai_addrlen);

        if(result == -1){
            perror("Socket Connection Failed");
            served_element = served_element->ai_next;
            continue;
        }

        break; // at this point we know the connection is good
        
    }

    freeaddrinfo(servinfo); 

    // even though we freed the mem, the pointer is still just hanging around...
    // spooky! this would never cause a security risk.
    if(served_element == NULL){
        fprintf(stderr, "Failed to connect to %s \n",url); // pray there is a termination char!
        return CONNECTIONFAIL;
    }

    // clean up pointers... doesn't really matter because they're about to go out of scope but whatever.
    served_element = NULL;
    servinfo = NULL;

    // output result
    *(sockfd_result) = sockfd;


return SUCCESS;
}