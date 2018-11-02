#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#define BUF 1024

#define UDS_FILE "/tmp/d16sim.uds"
using namespace std;

int main (int argc, char **argv) {
    int socked_fd;
    char buffer[BUF];
    struct sockaddr_un address = { AF_LOCAL, UDS_FILE };
    int size;
    if((socked_fd=socket (PF_LOCAL, SOCK_STREAM, 0)) < 0) {
        printf ("ERROR: Unable to create socket\n");
        exit(1);
    }
    //strcpy(address.sun_path, UDS_FILE);
    if (connect ( socked_fd, (struct sockaddr *) &address, sizeof (address))) {
        printf ("ERROR: Unable to connect to server\n");
        exit(1);
    }
    do {
        size = recv(socked_fd, buffer, BUF-1, 0);
        if( size > 0)
            buffer[size] = '\0';
        printf ("Nachricht erhalten: %s\n", buffer);
        if (strcmp (buffer, "quit\n")) {
            printf ("Nachricht zum Versenden: ");
            fgets (buffer, BUF, stdin);
            send(socked_fd, buffer, strlen (buffer), 0);
        }
    } while (strcmp (buffer, "quit\n") != 0);
    close (socked_fd);
    return EXIT_SUCCESS;
}
