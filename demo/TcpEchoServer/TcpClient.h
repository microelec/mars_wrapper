#ifndef TCPCLIENT_H_
#define TCPCLIENT_H_

#include <stddef.h>
#include <stdint.h>
#include <string>

class TcpClient
{
    std::string host_;
    uint16_t port_;
    int socket_desc_;
    bool is_connected_;
    void SocketShutdown();

public:
    TcpClient(std::string host, uint16_t port);
    virtual ~TcpClient();
    bool Connect();
    const std::string& getHost() const;
    uint16_t getPort() const;
    bool IsConnected() const;
    int Send(void *buffer, size_t size);
    int Recv(void *buffer, size_t size);
    int SendInt(uint32_t value);
};

#endif /* TCPCLIENT_H_ */

