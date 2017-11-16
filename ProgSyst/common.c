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

/* Vous pouvez ecrire ici toutes les fonctions */
/* qui pourraient etre utilisees par le lanceur */
/* et le processus intermediaire. N'oubliez pas */
/* de declarer le prototype de ces nouvelles */
/* fonctions dans common_impl.h */
