#ifndef __CLIENT_H
#define __CLIENT_H

#include <string>
#include <vector>

class UDSClient {
public:
    UDSClient( std::string fn );

    bool send( const std::vector<uint8_t> data );
    bool receive( std::vector<uint8_t>& data );

private:
    int m_socket_fd = -1;
};

#endif
