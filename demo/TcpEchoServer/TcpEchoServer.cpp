#include "TcpEchoServer.h"

SocketsListener* SocketsListener::m_instance=NULL;
SocketsListener* SocketsListener::singleInstance(){
    if(m_instance==NULL) m_instance = new SocketsListener();
    return m_instance;

}
SocketsListener::SocketsListener(){
    m_epollFd = epoll_create1(EPOLL_CLOEXEC);
    printf("epoll instance created %d \n",m_epollFd);

}
SocketsListener::~SocketsListener(){
    close(m_epollFd);
    printf("epoll instance closed %d \n",m_epollFd);
}

void SocketsListener::addSocketHandler(int fd, unsigned int events, Handler* handler){
    m_socketHandlers[fd] = handler;
    epoll_event e;
    e.data.fd = fd;
    e.events = events;

    int rst =epoll_ctl(m_epollFd, EPOLL_CTL_ADD, fd, &e); 
    if(rst < 0) {
        printf("Failed to insert handler to epoll:%d\n",rst);
    }

}
void SocketsListener::removeSocket(int fd ){
    m_socketHandlers.erase(fd);

}
void SocketsListener::updateSocket(int fd, unsigned int events){
    epoll_event e;
    e.data.fd = fd;
    e.events = events;

    int rst =epoll_ctl(m_epollFd, EPOLL_CTL_MOD, fd, &e); 
    if(rst < 0) {
        printf("Failed to update socket :%d\n",rst);
    }
}

void SocketsListener::run(){
    while(1){
        epoll_event events[MAX_EVENTS];
        int nfds = epoll_wait(m_epollFd, events, MAX_EVENTS, -1 /* Timeout */);
        for(int i = 0; i < nfds; ++i) {
            int fd = events[i].data.fd;
            if(m_socketHandlers.find(fd)!=m_socketHandlers.end()){
                Handler *h = m_socketHandlers[fd];
                h->handle(events[i]);
            }
        }
    }
}

ServerHandler::ServerHandler(const char* ip, const int port){
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    int fd;
    if ((fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        printf("Failed to create server socket \n");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    if (bind(fd, (struct sockaddr *) &addr,
                                 sizeof(addr)) < 0) {
        printf("Failed to bind server socket\n");
        exit(1);
    }

    if (listen(fd, 100) < 0) {
        printf("Failed to listen on server socket\n");
        exit(1);
    }
    //set non-blocking
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    m_fd =fd;
}
void ServerHandler::handle(epoll_event &e){
    sockaddr_in client_addr;
    socklen_t ca_len = sizeof(client_addr);

    int client = accept(m_fd, (struct sockaddr *) &client_addr,
                    &ca_len);

    if(client < 0) {
        printf("Failed to accept a new connection\n");
        return;
    }

    printf(">>>>client connected: %s\n",inet_ntoa(client_addr.sin_addr));
    Handler* handler=new EchoHandler(client);
    SocketsListener::singleInstance()->addSocketHandler(client,EPOLLIN,handler);
    return;

}
void EchoHandler::handle(epoll_event &e){
    int fd = m_fd;
    //int received=0;
    //char buffer[1000];
    if(e.events & EPOLLHUP) {
        SocketsListener::singleInstance()->removeSocket(fd);
        return;
    }

    if(e.events & EPOLLERR) {
        return;
    }

    if(e.events & EPOLLOUT) {
        if(m_received > 0) {
            printf("Sending %d bytes \n",m_received);
            if (send(fd, m_buffer, m_received, 0) != m_received) {
                printf("Error writing to socket\n");
            }
        }
        m_received = 0;
        SocketsListener::singleInstance()->updateSocket(fd, EPOLLIN);
    }

    if(e.events & EPOLLIN) {
        if ((m_received = recv(fd, m_buffer, sizeof(m_buffer), 0)) < 0) {
            printf("Error reading from socket \n");
        } else if(m_received > 0) {
            m_buffer[m_received] = 0;
            printf("Reading %d bytes: \n",m_received);
        }

        if(m_received > 0) {
            SocketsListener::singleInstance()->updateSocket(fd, EPOLLOUT);
        } else {
            SocketsListener::singleInstance()->removeSocket(fd);
        }
    }

    return;
}


//main entry
int main(int argc, char *argv[]) {
    const char* usage = "USAGE: ./TcpEchoServer port";
    if(argc!=2){
        printf("%s\n",usage);
        return -1;
    }
    int port = (int)atoi(argv[1]);
    printf("TcpEchoServer listening port %d\n",port);
    ServerHandler server("39.106.56.27",port);
    SocketsListener* listener = SocketsListener::singleInstance();
    listener->addSocketHandler(server.getFd(),EPOLLIN,&server);
    listener->run();
    printf("TcpEchoServer exit");

}
