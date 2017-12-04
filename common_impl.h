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

#define  MAX_LENGTH 1024 // Number of letters max in the message

#define ERROR_EXIT(str) {perror(str);exit(EXIT_FAILURE);}

/* definition du type des infos */
/* de connexion des processus dsm */
struct dsm_proc_conn  {
   int rank;
   struct in_addr dsm_addr;
   int dsm_port_num;
   /* a completer */
};
typedef struct dsm_proc_conn dsm_proc_conn_t;

/* definition du type des infos */
/* d'identification des processus dsm */

typedef struct dsm_proc {
  char* machine_name;
  pid_t pid;
  int sock;
  dsm_proc_conn_t connect_info;
  struct dsm_proc *next;
} dsm_proc_t;


typedef dsm_proc_t *list_dsm_proc;

int add_proc( list_dsm_proc *lst, char * machine_name);

int creer_socket(int type, int *port_num);
int do_socket(int domain, int type, int protocol);
void init_main_addr(struct sockaddr_in *serv_addr);
int do_bind(int serv_sock, struct sockaddr_in serv_addr, int serv_addr_len);
int do_accept(int sock, struct sockaddr_in *addr, socklen_t * addr_len);

ssize_t send_line(int fd, void *buf, size_t len);
ssize_t read_line(int fd, char * buf, size_t len);

void sigchld_handler( int sig);

/* Vous pouvez ecrire ici toutes les fonctions */
/* qui pourraient etre utilisees par le lanceur */
/* et le processus intermediaire. N'oubliez pas */
/* de declarer le prototype de ces nouvelles */
/* fonctions dans common_impl.h */
