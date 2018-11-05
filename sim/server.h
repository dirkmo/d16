#ifndef __SERVER_H
#define __SERVER_H

#include <string>
#include <vector>
#include <thread>

class UDSServer {
public:
    UDSServer( std::string fn );
    ~UDSServer();

    void start();
    
    bool send( const std::vector<uint8_t> data );
    bool receive( std::vector<uint8_t>& data );

private:
    void threadfunc();

    int m_socket_fd = -1;
    int m_client_fd = -1;
    struct sockaddr_un m_sockaddr;
    std::thread m_thread;
    bool m_bRunning = false;
    std::vector<uint8_t> m_vRecData;
};

#endif
