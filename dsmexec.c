#include "common_impl.h"

/* variables globales */

/* un tableau gerant les infos d'identification */
/* des processus dsm */
dsm_proc_t *proc_array = NULL;
/* le nombre de processus effectivement crees */
volatile int num_procs_creat = 0;
//typedef char nom_machines_t[20];

void usage(void)
{
  fprintf(stdout,"Usage : dsmexec machine_file executable arg1 arg2 ...\n");
  fflush(stdout);
  exit(EXIT_FAILURE);
}


int main(int argc, char *argv[])
{

  pid_t pid;
  int num_procs = 0, i, j, master_sock, *sock;
  struct sockaddr_in init_addr;
  char **args, *token, *newarguments;
  list_dsm_proc lst = NULL;

  if (argc < 3)
  {
    usage();
  }
  else
  {

    /* Mise en place d'un traitant pour recuperer les fils zombies*/

    struct sigaction action;
    memset( &action, 0, sizeof( struct sigaction));
    action.sa_handler = sigchld_handler; // pas besoin de parametre c'est un pointeur de fonction
    sigaction( SIGCHLD, &action, NULL);

    /* lecture du fichier de machines */
    /* 1- on recupere le nombre de processus a lancer */
    /* 2- on recupere les noms des machines : le nom de */
    /* la machine est un des elements d'identification */

    master_sock = creer_socket(SOCK_STREAM); // Create a socket with the domain, type and protocol given
    init_main_addr( &init_addr, &sock); // Initiation of the Server with a certain port
    do_bind( master_sock, init_addr, sizeof(init_addr)); // Binds a socket to an address
    listen( master_sock, num_procs);

    FILE * fp; // pointer on the file
    //FILE * start;
    char * line = NULL; // pointer on the beginning of the line in the file
    size_t len = 0; // length of the line
    ssize_t read; // number of read characters

    fp = fopen("./machine_file", "r");

    if (fp == NULL)
    {
      exit(EXIT_FAILURE);
    }

    while ((read = getline(&line, &len, fp)) != -1) // add machines to the table
    {
      token = strtok(line, "\n");
      add_proc(&lst,token);
      num_procs++;
    }
    free(line);
    fclose(fp);

    /* creation des fils */
    for(i = 0; i < num_procs ; i++)
    {
      /* creation du tube pour rediriger stdout */
      int out[2];
      pipe(out);

      /* creation du tube pour rediriger stderr */
      int err[2];
      pipe(err);

      pid = fork();

      if(pid == -1) ERROR_EXIT("fork");

      if (pid == 0) /* fils */
      {

        /* redirection stdout */
        close(out[0]);
        dup2(STDOUT_FILENO,out[1]);
        close(STDOUT_FILENO);

        /* redirection stderr */
        close(err[0]);
        dup2(STDERR_FILENO,err[1]);
        close(STDERR_FILENO);

        /* Creation du tableau d'arguments pour le ssh */
        strcpy(newarguments, newargv( argc, argv, init_addr));

        /* jump to new prog : */
        /* execvp("ssh",newarguments); */
        break;
      }
      else
      {
          /* fermeture des extremites des tubes non utiles */
          close(out[1]);
          dup2(out[0],STDOUT_FILENO);
          close(err[1]);
          dup2(err[0],STDERR_FILENO);
          num_procs_creat++;
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
