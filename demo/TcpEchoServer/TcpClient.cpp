#include "TcpClient.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <cstdio>
#include <cstring>

TcpClient::TcpClient(std::string host, uint16_t port) : host_(host),port_(port),socket_desc_(0),is_connected_(false)
{
}

TcpClient::~TcpClient()
{
        SocketShutdown();
}

const std::string& TcpClient::getHost() const
{
        return host_;
}

bool TcpClient::Connect()
{
        SocketShutdown();
            socket_desc_ = socket(AF_INET , SOCK_STREAM , 0);
                if (socket_desc_ != -1)
                        {
                                    struct sockaddr_in server;
                                            server.sin_addr.s_addr = inet_addr(host_.c_str());
                                                    server.sin_family = AF_INET;
                                                            server.sin_port = htons(port_);

                                                                    is_connected_ = connect(socket_desc_, (struct sockaddr *)&server , sizeof(server)) == 0;
                                                                        }
                    return is_connected_;
}

void TcpClient::SocketShutdown()
{
        if (socket_desc_ > 0)
                    shutdown(socket_desc_, SHUT_RDWR);
            socket_desc_ = 0;
                is_connected_ = false;
}

uint16_t TcpClient::getPort() const
{
        return port_;
}


bool TcpClient::IsConnected() const
{
        return is_connected_;
}

int TcpClient::Send(void* buffer, size_t size)
{
        return send(socket_desc_, buffer, size, 0);
}

int TcpClient::Recv(void* buffer, size_t size)
{
        return recv(socket_desc_, buffer, size, 0);
}

int TcpClient::SendInt(uint32_t value)
{
        uint32_t network_byte_order = htonl(value);
            return Send(&network_byte_order, sizeof(uint32_t));
}



int main(int argc , char *argv[])
{
    TcpClient* client = new TcpClient(argv[1], atoi(argv[2]));

    char server_reply[2000];

    if (!client->Connect())
    {
        puts("Connect failed");
    }
    else
    {
        puts("Connection successfully");
    }

    if (client->SendInt(strlen(argv[3])) < 0 || client->Send(argv[3], strlen(argv[3])) < 0)
    {
        puts("Send failed");
    }
    else
    {
        puts("Data Send");
    }

    if (client->Recv(server_reply, 2000) < 0)
    {
        puts("Recv failed");
    }
    else
    {
        puts("Reply received\n");
        puts(server_reply);
    }

    delete client;

    return 0;
}
