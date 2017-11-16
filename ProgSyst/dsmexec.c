#include "common_impl.h"

/* variables globales */

/* un tableau gerant les infos d'identification */
/* des processus dsm */
dsm_proc_t *proc_array = NULL;

/* le nombre de processus effectivement crees */
volatile int num_procs_creat = 0;

void usage(void)
{
  fprintf(stdout,"Usage : dsmexec machine_file executable arg1 arg2 ...\n");
  fflush(stdout);
  exit(EXIT_FAILURE);
}

void sigchld_handler( int sig)
{
  /* on traite les fils qui se terminent */
  /* pour eviter les zombies */

}


int main(int argc, char *argv[])
{
  if (argc < 3)
  {
    usage();
  }

  else
  {
    pid_t pid;
    int num_procs = 0;
    int *port_num, i, master_sock;
    struct sockaddr_in init_addr;

    /* Mise en place d'un traitant pour recuperer les fils zombies*/


    /* lecture du fichier de machines */
    /* 1- on recupere le nombre de processus a lancer */
    /* 2- on recupere les noms des machines : le nom de */
    /* la machine est un des elements d'identification */

    FILE * fp; // pointer on the file
    char * line = NULL; // pointer on the beginning of the line in the file
    size_t len = 0; // length of the line
    ssize_t read; // number of read characters

    fp = fopen("./machine_file", "r");
    int num_machines;
    num_machines = 0;

    if (fp == NULL)
    {
      exit(EXIT_FAILURE);
    }

    while ((read = getline(&line, &len, fp)) != -1)
    {
      exit(EXIT_FAILURE);
    }

    while ((read = getline(&line, &len, fp)) != -1) // read the number of lines in our machine file
    {
      num_machines =+ 1;
    }

    char tab_machines[num_machines]; // create a table in which we can find the machines names
    int index_machines = 0;

    while ((read = getline(&line, &len, fp)) != -1)
    { // add machines to the table
      tab_machines[index_machines] = line;
      index_machines =+ 1;
      printf("%s added to machines table \n", line);
    }

    if (line) // release the line pointer
    {
      free(line);
    }


    // Creation and listen of a socket

    master_sock = creer_socket(SOCK_STREAM, port_num); // Create a socket with the domain, type and protocol given
    init_main_addr( &init_addr, port_num); // Initiation of the Server with a certain port
    do_bind( master_sock, init_addr, sizeof(init_addr)); // Binds a socket to an address
    do_listen( master_sock);

    /* creation des fils */
    for(i = 0; i < num_procs ; i++)
    {

      /* creation du tube pour rediriger stdout */

      /* creation du tube pour rediriger stderr */

      pid = fork();
      if(pid == -1) ERROR_EXIT("fork");

      if (pid == 0) /* fils */
      {

        /* redirection stdout */

        /* redirection stderr */

        /* Creation du tableau d'arguments pour le ssh */

        /* jump to new prog : */
        /* execvp("ssh",newargv); */

      }
      else
      {
        if(pid > 0) /* pere */
        {
          /* fermeture des extremites des tubes non utiles */
          num_procs_creat++;
        }
      }
    }


    for(i = 0; i < num_procs ; i++)
    {

      /* on accepte les connexions des processus dsm */

      /*  On recupere le nom de la machine distante */
      /* 1- d'abord la taille de la chaine */
      /* 2- puis la chaine elle-meme */

      /* On recupere le pid du processus distant  */

      /* On recupere le numero de port de la socket */
      /* d'ecoute des processus distants */
    }

    /* envoi du nombre de processus aux processus dsm*/

    /* envoi des rangs aux processus dsm */

    /* envoi des infos de connexion aux processus */

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
