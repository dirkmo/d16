#ifndef __SERVER_H
#define __SERVER_H

#include <string>
#include <vector>

class UDSServer {
public:
    UDSServer( std::string fn );
    ~UDSServer();
    
    void wait();
    bool send( const std::vector<uint8_t> data );
    bool receive( std::vector<uint8_t>& data );

private:
    int m_socket_fd = -1;
    int m_client_fd = -1;
    struct sockaddr_un m_sockaddr;
};

#endif
