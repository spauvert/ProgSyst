#include "common_impl.h"

int main(int argc, char **argv)
{
  int port_wrap, sock_wrap, buffer_int, status, j;
  struct addrinfo hints, *res, *rp;
  char **exec_args = NULL;
  printf("1\n");
   /* processus intermediaire pour "nettoyer" */
   /* la liste des arguments qu'on va passer */
   /* a la commande a executer vraiment */

   /*
   newargv[0], ssh
   newargv[1], nom de la machine
   newargv[2], dsmwrap = argv[0]
   newargv[3], @IP de la machine = argv[1]
   newargv[4], Numero du port machine = argv[2]
   newargv[5], prog à executer = argv[3]
   newargv[6+], the other arguments = argv[4+]

   }*/

   /* creation d'une socket pour se connecter au */
   /* au lanceur et envoyer/recevoir les infos */
   /* necessaires pour la phase dsm_init */

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    status = getaddrinfo(argv[1], argv[2], &hints, &res); // Creates a chained list of possible servers that the client can connect to

    if (status != 0)
    {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
      exit(EXIT_FAILURE);
    }

    for (rp = res; rp != NULL; rp = rp->ai_next)
    { // Loop continues until it make a connection with the right server
      sock_wrap = socket(rp->ai_family, rp->ai_socktype,rp->ai_protocol); // Creates a sock at start of loop

      if (sock_wrap == -1) // If the connection fails, go to next possible server
      continue;

      if (connect(sock_wrap, rp->ai_addr, rp->ai_addrlen) != -1) // If the connection is a success then we quit the loop
      break;

      close(sock_wrap); // Sock is closed if the connection was not able to be made, avoids having lots of sockets open
    }

    if (rp == NULL)
    {  // If no possible server is avaiable, then error is sent
      perror("Client could not connect to dsmexec.\n");
    }

   /* Envoi du nom de machine au lanceur */


   /* Envoi du pid au lanceur */
   buffer_int = getpid();
   send_line(sock_wrap, &buffer_int,sizeof(buffer_int) );

   /* Creation de la socket d'ecoute pour les */
   /* connexions avec les autres processus dsm */
   creer_socket(SOCK_STREAM, &port_wrap);

   /* Envoi du numero de port au lanceur */
   /* pour qu'il le propage � tous les autres */
   /* processus dsm */

   exec_args = malloc((argc-3+1)*sizeof(char*));

   for (j = 0; j < argc-3; j++)
   {
     exec_args[j] = argv[j+3]; // The other arguments
   }

   /*
   exec_args[0] = argv[3]

   exec_args[1+] = argv[4+]
   */
   /* on execute la bonne commande */
   execvp(exec_args[0], exec_args);

   return 0;
}
