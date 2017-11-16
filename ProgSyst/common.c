#include "common_impl.h"

int creer_socket(int type, int *port_num)
{
   int fd = socket(AF_INET,type,IPPROTO_TCP); // A socket is created here
   int yes = 1;

   if (fd == -1) // if there is an error, then
   {
     perror( "Error socket couldnt be created" ); // a message will be sent
   }

   if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
   {
     perror("ERROR setting socket options");
   }

   *port_num = rand()%65535+1024;

   return fd;
}

void init_main_addr(struct sockaddr_in *serv_addr, char* serv_port) // Initialises the server
{
  memset(serv_addr, 0, sizeof(serv_addr));
  serv_addr->sin_family = AF_INET; // Family type is set
  serv_addr->sin_port = htons( atoi( serv_port)); // Port number is set
  inet_aton("127.0.0.1", &serv_addr->sin_addr); // IP address is set
}

int do_bind(int serv_sock, struct sockaddr_in serv_addr, int serv_addr_len) // Bind a socket and server_addrese
{
  int serv_bind = bind(serv_sock, (struct sockaddr *) &serv_addr, serv_addr_len); // A sock and the IP address of server are bound together

  if ( serv_bind == -1 ) // if there is an error, then
  {
    error("bind"); // a message is sent
  }
  return( EXIT_SUCCESS );
}
/* Vous pouvez ecrire ici toutes les fonctions */
/* qui pourraient etre utilisees par le lanceur */
/* et le processus intermediaire. N'oubliez pas */
/* de declarer le prototype de ces nouvelles */
/* fonctions dans common_impl.h */
