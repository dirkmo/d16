#include "client.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cassert>
#include <iostream>

using namespace std;

UDSClient::UDSClient( string fn ) {
    m_socket_fd = socket( PF_LOCAL, SOCK_STREAM, 0 );
    
    struct sockaddr_un address;
    address.sun_family = AF_LOCAL;
    strcpy( address.sun_path, fn.c_str() );

    int ret = connect( m_socket_fd, (struct sockaddr *) &address, sizeof (address) );
    if( ret != 0) {
        close(m_socket_fd);
        m_socket_fd = -1;
    }
    cout << "UDSClient connected." << endl;
}

UDSClient::~UDSClient() {
    if( m_socket_fd > -1 ) {
        close(m_socket_fd);
    }
}

bool UDSClient::send( vector<uint8_t> data ) {
    assert( m_socket_fd > -1 );
    return ::send(m_socket_fd, data.data(), data.size(), 0) == data.size();
}

bool UDSClient::receive( vector<uint8_t>& data ) {
    assert( m_socket_fd > -1 );
    uint8_t dat;
    ssize_t size;
    while( (size = recv( m_socket_fd, &dat, 1, MSG_DONTWAIT ) ) > 0 ) {
        data.push_back(dat);
    }
    return data.size() > 0;
}

#if 0
int main() {
    UDSClient uds("/tmp/d16sim.uds");
    uds.send( { 'T', 'e', 's', 't' } );
}
#endif
