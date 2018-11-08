#ifndef __CLIENT_H
#define __CLIENT_H

#include <string>
#include <vector>
#include <stdint.h>

class UDSClient {
public:
    UDSClient( std::string fn );
    ~UDSClient();
    
    bool connect();
    bool send( const std::vector<uint8_t> data );
    bool receive( std::vector<uint8_t>& data );

private:
    int m_socket_fd = -1;
    std::string m_sSocketName;
};

#endif
