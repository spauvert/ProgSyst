#include "common_impl.h"

/* variables globales */

/* un tableau gerant les infos d'identification */
/* des processus dsm */

// dsm_proc_t *proc_array = NULL;

/* le nombre de processus effectivement crees */
volatile int num_procs_creat = 0;
//typedef char nom_machines_t[20];

void usage(void)
{
  fprintf(stdout,"Usage : dsmexec machine_file executable arg1 arg2 ...\n");
  fflush(stdout);
  exit(EXIT_FAILURE);
}

void sigchld_handler( int sig)
{
  while (waitpid( -1, NULL, WNOHANG) < 0)
  {
      num_procs_creat--;
  }
}

int main(int argc, char *argv[])
{
  struct sockaddr_in init_addr, client_addr;
  char *token, message[ MAX_LENGTH], **newargv = NULL, * line = NULL, str[1024], exec_path[1024], *wd_ptr = NULL;
  struct hostent *res;
  fd_set readfds;
  int num_procs = 0, i, j, master_sock, new_socket, max_sock;
  socklen_t client_addr_len = sizeof(client_addr);
  pid_t pid;
  list_dsm_proc lst = NULL;
  dsm_proc_t *listing1 = NULL, *listing2 = NULL;
  FILE * fp;
  size_t len = 0;
  ssize_t read;

  if (argc < 3)
  {
    usage();
  }
  else
  {

    /* Mise en place d'un traitant pour recuperer les fils zombies*/

    struct sigaction action;
    memset( &action, 0, sizeof( struct sigaction));
    action.sa_handler = sigchld_handler;
    sigaction( SIGCHLD, &action, NULL);

    /* lecture du fichier de machines */

    wd_ptr = getcwd(str,1024);
    sprintf(exec_path,"%s/%s",str,argv[1]);
    fp = fopen(exec_path, "r");

    if (fp == NULL)
    {
      exit(EXIT_FAILURE);
    }

    /*
    1- on recupere le nombre de processus a lancer
    2- on recupere les noms des machines : le nom de
    la machine est un des elements d'identification
    */

    while ((read = getline(&line, &len, fp)) != -1)
    {
      token = strtok(line, "\n");
      add_proc(&lst,token);
      num_procs++;
    }
    free(line);
    fclose(fp);

    /* Socket Creation */

    master_sock = do_socket( AF_INET, SOCK_STREAM, IPPROTO_TCP);
    init_main_addr( &init_addr);
    printf("%s and %u\n", inet_ntoa(init_addr.sin_addr), ntohs(init_addr.sin_port));
    do_bind( master_sock, init_addr, sizeof(init_addr));
    listen( master_sock, num_procs);

    return (0);

    /* Son Creation */

    listing1 = lst;

    for(i = 0; i < num_procs ; i++)
    {
      /* creation du tube pour rediriger stdout */
      //int out[2];
      //pipe(out);

      /* creation du tube pour rediriger stderr */
      //int err[2];
      //pipe(err);

      pid = fork();

      if(pid == -1) ERROR_EXIT("fork");

      if (pid == 0) /* Son */
      {
        // redirection stdout
        /*close(out[0]);
        dup2(STDOUT_FILENO,out[1]);
        close(STDOUT_FILENO);

        // redirection stderr
        close(err[0]);
        dup2(STDERR_FILENO,err[1]);
        close(STDERR_FILENO);*/
        listing1->pid = getpid();

        //printf("name:%s, pid: %i\n",listing1->machine_name, listing1->pid);

        /* Creation du tableau d'arguments pour le ssh */

        newargv = malloc((argc+4)*sizeof(char*));
        for ( j = 0; j < argc+3; j++)
        {
          newargv[j] = malloc(200*sizeof(char));
        }

        sprintf(newargv[0], "ssh"); // ssh
        sprintf(newargv[1], "%s", listing1->machine_name); // nom de la machine
        sprintf(newargv[2], "%s/%s",str,"./bin/dsmwrap");
        sprintf(newargv[3], "%s", inet_ntoa(init_addr.sin_addr)); // @IP de la machine
        sprintf(newargv[4], "%d", ntohs(init_addr.sin_port)); // Numero du port machine
        sprintf(newargv[5], "%s", argv[2]); // prog à executer

        if (argc > 3)
        {
          for (j = 0; j < argc-3; j++)
          {
            newargv[6+j] = argv[j+3]; // The other arguments
          }
        }
        printf("%s\n",newargv[0] );
        printf("%s\n",newargv[1] );
        printf("%s\n",newargv[2] );
        printf("%s\n",newargv[3] );
        printf("%s\n",newargv[4] );
        printf("%s\n",newargv[5] );


        /* jump to new prog :*/
       if (execvp(newargv[0], newargv) == -1)
       {
         ERROR_EXIT("exec child dsmexec");
       }
      }
      else // Father
      {
        if (pid > 0){
          /* fermeture des extremites des tubes non utiles */
          /*close(out[1]);
          dup2(out[0],STDOUT_FILENO);
          close(err[1]);
          dup2(err[0],STDERR_FILENO);*/
          num_procs_creat++;
          //printf("num_procs_creat: %i\n",num_procs_creat);
          listing1 = listing1->next;
        }
      }
    }

    FD_ZERO(&readfds); //Clears the socket set
    FD_SET(master_sock, &readfds); //Add the server's main socket
    max_sock = master_sock;

    for (i = 0; i < num_procs ; i++)
    {
      listing1 = lst;

      if (select( max_sock + 1 , &readfds , NULL , NULL , NULL) < 0) // Select a socket where there is "mouvement" or activity
      {
        ERROR_EXIT("select");
      }

      if (FD_ISSET(master_sock, &readfds)) // If something happens on the master socket (aka serv_sock), then it means there is an incoming connection
      {
        /* on accepte les connexions des processus dsm */
        new_socket =  do_accept( master_sock, &client_addr, &client_addr_len);

        /*  On recupere le nom de la machine distante */
        res = gethostbyaddr(&client_addr.sin_addr, sizeof(client_addr.sin_addr), AF_INET);

        /* 1- d'abord la taille de la chaine */
        char * distant_mach_name = (char *) malloc((strlen(res->h_name) +1) * sizeof(char));

        /* 2- puis la chaine elle-meme */
        strcpy(distant_mach_name, res->h_name);

        while ( strcmp(listing1->machine_name, distant_mach_name) != 0 ) {
          listing1 = listing1->next;
        }

        /* On recupere le pid du processus distant  */
        read = read_line( new_socket, message, MAX_LENGTH);

        while( listing1 != NULL )
        {
          if (strcmp(listing1->machine_name, distant_mach_name) == 0 && listing1->connect_info.rank == 0)
          {
            /* On recupere le numero de port de la socket */
            /* d'ecoute des processus distants */
            listing1->pid = atoi(message);
            listing1->connect_info.rank = i;
            listing1->connect_info.dsm_addr = client_addr.sin_addr;
            listing1->connect_info.dsm_port_num = client_addr.sin_port;
            listing1->sock = new_socket;
            FD_SET(new_socket, &readfds);
            break;
          }
          else
          {
            listing1 = listing1->next;
          }
        }
        if (listing1 == NULL)
        {
          close(new_socket);
        }
        memset( message, 0, sizeof(message));
      }
    }
    for (i = 0; i < num_procs ; i++)
    {
      listing2 = lst;
      /* envoi du nombre de processus aux processus dsm*/
      sprintf(message, "%i", num_procs_creat);
      send_line(listing1->sock, message, sizeof(message));

      /* envoi des rangs aux processus dsm */
      sprintf(message, "%i", listing1->connect_info.rank);
      send_line(listing1->sock, message, sizeof(message));

      /* envoi des infos de connexion aux processus */
      for (j = 0; j < num_procs; j++) {
        if ( strcmp(listing1->machine_name,listing2->machine_name) != 0)
        {
          sprintf(message, "%i", listing1->connect_info.rank);
          send_line(listing2->sock, message, sizeof(message));

          sprintf(message, "%s", inet_ntoa (listing1->connect_info.dsm_addr));
          send_line(listing2->sock, message, sizeof(message));

          sprintf(message, "%i", listing1->connect_info.dsm_port_num);
          send_line(listing2->sock, message, sizeof(message));
        }
        listing2 = listing2->next;
      }

      listing1 = listing1->next;
    }

    /* gestion des E/S : on recupere les caracteres */
    /* sur les tubes de redirection de stdout/stderr */
    /* while(1)
    {
    je recupere les infos sur les tubes de redirection
    jusqu'� ce qu'ils soient inactifs (ie fermes par les
    processus dsm ecrivains de l'autre cote ...)

  };
  */

  /* on attend les processus fils */

  /* on ferme les descripteurs proprement */

  /* on ferme la socket d'ecoute */
}
exit(EXIT_SUCCESS);
}
