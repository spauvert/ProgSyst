#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/mman.h>

#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>

#include <arpa/inet.h>
#include <time.h>


/* autres includes (eventuellement) */

#define ERROR_EXIT(str) {perror(str);exit(EXIT_FAILURE);}

/* definition du type des infos */
/* de connexion des processus dsm */
struct dsm_proc_conn  {
   int rank;
   /* a completer */
};
typedef struct dsm_proc_conn dsm_proc_conn_t;

/* definition du type des infos */
/* d'identification des processus dsm */

typedef struct dsm_proc {
  char* machine_name;
  pid_t pid;
  dsm_proc_conn_t connect_info;
  struct dsm_proc *next;
} dsm_proc_t;


typedef dsm_proc_t *list_dsm_proc;

int add_proc(list_dsm_proc *lst, char * machine_name);
void init_list_dsm_proc(list_dsm_proc * lst); // Initialises the list of processus

char *newargv( char* machine_name, struct sockaddr_in init_addr);

int creer_socket(int type);
void init_main_addr(struct sockaddr_in *serv_addr, u_short* sock);
int do_bind(int serv_sock, struct sockaddr_in serv_addr, int serv_addr_len);
void sigchld_handler( int sig);
