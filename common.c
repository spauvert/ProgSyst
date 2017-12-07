#include "common_impl.h"

volatile uint64_t num_fils = 0;

int add_proc( list_dsm_proc *lst, char * machine_name)
{
  dsm_proc_t *p = (dsm_proc_t *) malloc(sizeof(dsm_proc_t));

  if (p != NULL)
  {
    p->machine_name = (char *) malloc((strlen(machine_name) +1) * sizeof(char)); // creates sapce for the nickname
    if (p->machine_name != NULL)
    {
      strcpy( p->machine_name, machine_name);
      p->connect_info.rank = 0;
      p->pid = 0;
      p->next = *lst;
      *lst = p;
    }
    else
    {
      free(p);
      p = NULL;
    }
  }
  return(p != NULL);
}

int creer_socket(int prop, int *port_num)
{
  struct sockaddr_in init_addr;

  int sock = do_socket( AF_INET, prop, IPPROTO_TCP);  // Create a socket with the domain, type and protocol given
  init_main_addr( &init_addr); // Initiation of the Server with a certain port
  do_bind( sock, init_addr, sizeof(init_addr)); // Binds a socket to an address

  *port_num = ntohs(init_addr.sin_port);

  return sock;
}

int do_socket(int domain, int type, int protocol) // Creates a socket
{
  int sock = socket(domain,type,protocol); // A socket is created here
  int yes = 1;

  if (sock == -1) // if there is an error, then
  {
    ERROR_EXIT( "Error socket couldnt be created" ); // a message will be sent
  }

  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
  {
    ERROR_EXIT("ERROR setting socket options");
  }

  return sock;
}

void init_main_addr(struct sockaddr_in *serv_addr) // Initialises the server
{
  memset(serv_addr, 0, sizeof(*serv_addr));
  serv_addr->sin_family = AF_INET; // Family type is set
  serv_addr->sin_port = htons(0); // Port number is set
  serv_addr->sin_addr.s_addr = htons(INADDR_ANY); // IP address is set
}

int do_bind(int serv_sock, struct sockaddr_in serv_addr, int serv_addr_len) // Bind a socket and server_addrese
{
  int serv_bind = bind(serv_sock, (struct sockaddr *) &serv_addr, serv_addr_len); // A sock and the IP address of server are bound together

  if ( serv_bind == -1 ) // if there is an error, then
  {
    perror("bind"); // a message is sent
  }
  return( EXIT_SUCCESS );
}

int do_accept(int sock, struct sockaddr_in *addr, socklen_t * addr_len) // accepts a client that wants to join the server
{
  int serv_accept = accept(sock, (struct sockaddr *) &addr, addr_len); // after listening on the socket, once it picks up a signal it can accept it

  if ( serv_accept == -1 )  // if there is an error, then ...
  {
    perror( "do_accept:"); // an message is sent
  }

  return serv_accept;
}

ssize_t send_line(int fd, void *buf, size_t len) // Sends a message
{
  int nleft;
  int nwritten;
  char * ptr;
  ptr = buf;
  nleft = len;

  while( nleft > 0 ) // we go through the length of the message
  {
    if( (nwritten = write(fd, ptr, nleft)) <= 0) // nwritten amount of the message will be sent at a time,
    {
      if(errno == EINTR)
      {
        nwritten = 0;
        perror("");
      }
      return -1;
    }
    nleft -= nwritten; // we remove the amount sent from the amount to be sent
    ptr += nwritten; // we had the amount sent to the pointer on the beginning of the message
  }
  return len;
}

ssize_t read_line(int fd, void * buf, size_t len) // sends a message
{
  int i;
  char c;
  int ret;
  char * ptr;
  ptr = buf;
  int cnt = 0;

  for (i = 0 ; i < len; i++) // we go through one by one til the end of the max length of a message
  {

    ret = read(fd, &c, 1); // We read the message received letter by letter one at a time

    if( ret == 1 )
    {
      ptr[cnt++] = c; // the letter is put into ptr

      if( c == '\n') // if we reach a \n ( = "enter" on keybard"), we leave the loop
      {
        ptr[cnt] = '\0';
        return i+1;
        break;
      }
    }
    else if( 0 == ret  ) // we reach the end of the message, we leave the loop
    {

      ptr[cnt] = '\0';
      break;
    }
  }
  ptr[len] = '\0';

  return i;
}

/* Vous pouvez ecrire ici toutes les fonctions */
/* qui pourraient etre utilisees par le lanceur */
/* et le processus intermediaire. N'oubliez pas */
/* de declarer le prototype de ces nouvelles */
/* fonctions dans common_impl.h */
