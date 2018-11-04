#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include "server.h"

using namespace std;


UDSServer::UDSServer( std::string fn ) {
    m_socket_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    unlink( fn.c_str() );
    m_sockaddr.sun_family = AF_LOCAL;
    strcpy(m_sockaddr.sun_path, fn.c_str());
    int ret = bind( m_socket_fd, (struct sockaddr *)&m_sockaddr, sizeof(m_sockaddr) );
    if( ret != 0 ) {
        throw runtime_error("ERROR: Cannot bind to socket");
    }
    listen( m_socket_fd, 0 );
}

UDSServer::~UDSServer() {
    if( m_client_fd > -1 ) {
        close(m_client_fd);
    }
    if( m_socket_fd > -1 ) {
        close(m_socket_fd);
    }
}

void UDSServer::wait() {
    socklen_t addrlen;
    m_client_fd = accept ( m_socket_fd, (struct sockaddr *) &m_sockaddr, &addrlen );
}

bool UDSServer::send( const std::vector<uint8_t> data ) {
    if( m_client_fd > -1 ) {
        return ::send (m_client_fd, data.data(), data.size(), 0) == data.size();
    }
    return false;
}

bool UDSServer::receive( std::vector<uint8_t>& data ) {
    bool rec = false;
    if( m_client_fd > -1 ) {
        uint8_t buf[64];
        ssize_t size;
        data.clear();
        while( (size = recv( m_client_fd, &buf, sizeof(buf), MSG_DONTWAIT ) ) > 0 ) {
            for( int i = 0 ; i < size; i++ ) {
                data.push_back(buf[i]);
            }
            rec |= size > 0;
        }
    }
    return rec;
}

int main() {
    cout << "d16sim console" << endl;
    UDSServer uds("/tmp/d16sim.uds");
    cout << "Waiting for connection to simulator...";
    uds.wait();
    cout << "connected." << endl;
    vector<uint8_t> data;
    while(1) {
        usleep( 1000 );
        if( uds.receive( data ) ) {
            for( auto d: data ) {
                cout << d;
            }
            cout << flush;
        }
    }
    return 0;
}