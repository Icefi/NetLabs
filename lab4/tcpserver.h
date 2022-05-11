#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define BUF_SIZE 1024
#define MAX_SERV 16

struct TCPServer {
    int listenfd;

    struct sockaddr_in servAddr;

    int clients [FD_SETSIZE];
    int maxfd, maxi, connfd;
    fd_set rset, allset;

    void (*on_start)(struct TCPServer*);
    void (*on_update)(struct TCPServer*);
    void (*on_recv)(struct TCPServer*, char*, int);
    void (*on_send)(struct TCPServer*, char*, int);
    void (*on_shutdown)(struct TCPServer*);
};

typedef struct TCPServer TCPServer_t;

extern int  TCPServer_Init (TCPServer_t *self);
extern int  TCPServer_Run (TCPServer_t *self);
extern int  TCPServer_Terminate (TCPServer_t *self);
extern void TCPServer_DisplayServerInfo(TCPServer_t *self);

