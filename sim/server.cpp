#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
#include <iostream>

#include "server.h"

using namespace std;


UDSServer::UDSServer( std::string fn ) : m_thread() {
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
    m_thread.join();
}

void UDSServer::start() {
    m_thread = thread( &UDSServer::threadfunc, this );
}

void UDSServer::threadfunc() {
    uint8_t buf[128];
    m_bRunning = true;
    while(m_bRunning) {
        socklen_t addrlen;
        //cout << endl << "Ready." << endl;
        m_client_fd = accept ( m_socket_fd, (struct sockaddr *) &m_sockaddr, &addrlen );
        //cout << "connected." << endl;
        while( m_client_fd > -1 ) {
            ssize_t size = recv( m_client_fd, buf, sizeof(buf), 0 );
            if( size > 0 ) {
                m_mutex.lock();
                for( ssize_t i = 0; i<size; i++ ) {
                    m_vRecData.push_back(buf[i]);
                }
                m_mutex.unlock();
            } else  {
                close(m_client_fd);
                m_client_fd = -1;
                //cout << endl << "Disconnected." << endl;
            }
        }
    }
}

bool UDSServer::send( const std::vector<uint8_t> data ) {
    if( m_client_fd > -1 ) {
        return ::send( m_client_fd, data.data(), data.size(), 0 ) == static_cast<ssize_t>(data.size());
    }
    return false;
}

bool UDSServer::receive( std::vector<uint8_t>& data ) {
    bool ret = false;
    m_mutex.lock();
    if( m_vRecData.size() ) {
        data = m_vRecData;
        m_vRecData.clear();
        ret = true;
    }
    m_mutex.unlock();
    return ret;
}

int main() {
    cout << "d16sim console" << endl;
    UDSServer uds("/tmp/d16sim.uds");
    uds.start();
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
