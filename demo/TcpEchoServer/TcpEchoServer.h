#include <iostream>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <fcntl.h>
// epoll event handler interface
class Handler{
public:
    virtual void handle(epoll_event& e)=0;
    virtual int getFd(){return m_fd;}
    int m_fd;
};

//server which handle the server port socket
class ServerHandler:public Handler{
public:
    ServerHandler(const char* ip,const int port);
    virtual void handle(epoll_event& e);
};

//echo socket handler
class EchoHandler:public Handler{
public:
    EchoHandler(int fd){Handler::m_fd=fd;}
    virtual void handle(epoll_event& e);
private:
    int m_received;
    char m_buffer[1000];
};


//Sockets multiplex listener
class SocketsListener{
public:
    static const unsigned int MAX_EVENTS=5;
    static SocketsListener* singleInstance();
    SocketsListener();
    ~SocketsListener();
    void run();
    void addSocketHandler(int fd,unsigned int events,Handler* handler);
    void removeSocket(int fd);
    void updateSocket(int fd,unsigned int events);


private:
    static SocketsListener* m_instance;
    int m_epollFd;
    std::map<int,Handler*> m_socketHandlers;
    //struct epoll_event m_activeEvents[kMaxEvents];

};

