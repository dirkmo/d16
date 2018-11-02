#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#define BUF 1024
#define UDS_FILE "/tmp/d16sim.uds"

int main (void) {
  int create_socket, new_socket;
  socklen_t addrlen;
  char buffer[BUF];
  ssize_t size;
  struct sockaddr_un address;
  const int y = 1;
  if((create_socket=socket (AF_LOCAL, SOCK_STREAM, 0)) > 0)
    printf ("Socket wurde angelegt\n");
  unlink(UDS_FILE);
  address.sun_family = AF_LOCAL;
  strcpy(address.sun_path, UDS_FILE);
  if (bind ( create_socket,
             (struct sockaddr *) &address,
             sizeof (address)) != 0) {
    printf( "Der Port ist nicht frei – belegt!\n");
  }
  listen (create_socket, 5);
  addrlen = sizeof (struct sockaddr_in);
  while (1) {
     new_socket = accept ( create_socket,
                           (struct sockaddr *) &address,
                           &addrlen );
     if (new_socket > 0)
      printf ("Ein Client ist verbunden ...\n");
     do {
        printf ("Nachricht zum Versenden: ");
        fgets (buffer, BUF, stdin);
        send (new_socket, buffer, strlen (buffer), 0);
        size = recv (new_socket, buffer, BUF-1, 0);
        if( size > 0)
           buffer[size] = '\0';
        printf ("Nachricht empfangen: %s\n", buffer);
     } while (strcmp (buffer, "quit\n") != 0);
     close (new_socket);
  }
  close (create_socket);
  return EXIT_SUCCESS;
}
