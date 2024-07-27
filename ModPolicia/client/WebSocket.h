//https://github.com/sjfricke/Android-NDK-WebSocket/blob/master/demo/demo.cpp

#ifndef ANDROID_NDK_WEBSOCKET_H
#define ANDROID_NDK_WEBSOCKET_H

#include <cstring>
#include <string>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <thread> // std threads instead of pthreads due to c++ member function issues

#define MAX_MESSAGE_BUFFER 1024

// function pointer array where the message is the passed in arg
typedef void (*event_map_t)(char*);

class WebSocket {
  
 public:
    
    // Constructor and destructor
    WebSocket(int message_keys = 16);
    
    ~WebSocket();

    // used to setup and connect to server
    // returns 0 on success
    int connectSocket(std::string ip, int port);

    // sends message to all other users online
    // returns 0 on success
    int broadcast(int key, int option, std::string message);

    // sets up a event listener by passing the function and message key to map it to
    // returns 0 on success
    int setEvent(int key, void (*callbackFunction)(char*));

    // called on key == -1
    // passes in uid of client
    // returns 0 on success
    int setJoinEvent(void (*callbackFunction)(int));

    // called on key == -2
    // passes in uid of client
    // returns 0 on success
    int setLeaveEvent(void (*callbackFunction)(int));
    
 private:

    event_map_t* response_map;    // array map of event callbacks
    void (*on_join)(int);	  // when key == -1 
    void (*on_leave)(int);        // when key == -2
    
    struct sockaddr_in server_addr; // socket struct object
    int socket_fd;                  // holds socket file discriptor
    char msg_buffer_out[MAX_MESSAGE_BUFFER];     // outgoing messages
    int max_message_keys;
    
    std::thread rec_thread; 
    // runs a seperate thread to wait for incoming request
    void messageThread();
    
};

#endif // ANDROID_NDK_WEBSOCKET_H
