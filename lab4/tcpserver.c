#include "tcpserver.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <signal.h>

#define BUF_SIZE 1024

static void default_start(TCPServer_t* self) {
    #ifdef DEBUG
        printf("SERVER STARTED!\n");
    #endif
}

static void default_update(TCPServer_t* self) {
    #ifdef DEBUG
        printf("SERVER UPDATED\n");
    #endif
}

static void default_recv(TCPServer_t* self, char* buf, int socket) {
    #ifdef DEBUG
        printf("CLIENT[%d] > %s\n", socket, buf);
    #endif
}

static void default_send(TCPServer_t* self, char* buf, int socket) {
    #ifdef DEBUG
        printf("SERVER > Message sent!\n");
    #endif
}

static void default_shutdown(TCPServer_t* self) {
    #ifdef DEBUG
        printf("\nSERVER IS CLOSED\n");
    #endif
}

int TCPServer_Init(TCPServer_t *self) {

    unsigned int addrLen;

    self -> on_start    = default_start;
    self -> on_update   = default_update;
    self -> on_recv     = default_recv;
    self -> on_send     = default_send;
    self -> on_shutdown = default_shutdown;

    if ((self -> listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return 2;

    self -> servAddr.sin_family = AF_INET;
    self -> servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    self -> servAddr.sin_port = 0;

    if (bind(self -> listenfd, (struct sockaddr*) &self -> servAddr, sizeof self -> servAddr))
        return 3;

    addrLen = sizeof self -> servAddr;
    if (getsockname(self -> listenfd, (struct sockaddr*) &self -> servAddr, &addrLen))
        return 4;

    listen(self -> listenfd, 5);

    self -> maxfd = self -> listenfd;
    self -> maxi  = -1;

    for (int i = 0; i < FD_SETSIZE; i++)
        self -> clients[i] = -1;

    FD_ZERO(&self -> allset);
    FD_SET(self -> listenfd, &self -> allset);


    return 0;
}

int TCPServer_Run(TCPServer_t *self) {

    self -> on_start(self);

    int i, sockfd, nready;

    struct sockaddr_in cliaddr;
    unsigned int clilen, n;

    char buffer [BUF_SIZE] = "";

    for (;;) {
        self -> on_update(self);

        self -> rset = self -> allset;
        nready = select(self -> maxfd + 1, &self -> rset, NULL, NULL, NULL);

        if (FD_ISSET(self -> listenfd, &self -> rset)) {
            clilen = sizeof(cliaddr);
            self -> connfd = accept(self -> listenfd, (struct sockaddr*)&cliaddr, &clilen);

            for (i = 0; i < FD_SETSIZE; i++) {
                if (self -> clients[i] < 0) {
                    self -> clients[i] = self -> connfd;
                    break;
                }
            }

            if (i == FD_SETSIZE)
                return 1;

            FD_SET(self -> connfd, &self -> allset);
            if (self -> connfd > self -> maxfd)
                self -> maxfd = self -> connfd;

            if (i > self -> maxi)
                self -> maxi = i;

            if (--nready <= 0)
                continue;
        }

        for (i = 0; i <= self -> maxi; i++) {
            if ((sockfd = self -> clients[i]) < 0)
                continue;

            if (FD_ISSET(sockfd, &self -> rset)) {
                if ((n = recv(sockfd, &buffer, BUF_SIZE, 0)) == 0) {
                    #ifdef DEBUG
                        printf("\nSERVER > client[%d] disconnected\n", sockfd);
                    #endif

                    close(sockfd);
                    FD_CLR(sockfd, &self -> allset);
                    self -> clients[i] = -1;
                }

                buffer[n] = '\0';

                TCPServer_DisplayServerInfo(self);
                self -> on_recv(self, buffer, sockfd);


                send(sockfd, buffer, strlen(buffer), 0);

                self -> on_send(self, buffer, sockfd);

                if (--nready <= 0)
                    break;
            }
        }
    }
}

int TCPServer_Terminate (TCPServer_t *self) {

    self -> on_shutdown(self);

    for(int i = 0; i <= self -> maxi; i++) {
        if(self -> clients[i] != -1) {

            #ifdef DEBUG
                printf("SERVER: Client[%d] disconnected\n", self -> clients[i]);
            #endif

            close(self -> clients[i]);
        }
    }
    close(self -> listenfd);

    return 0;
}

void TCPServer_DisplayServerInfo(TCPServer_t *self) {
    printf("SERVER > %s : %d\n", inet_ntoa(self -> servAddr.sin_addr), ntohs(self -> servAddr.sin_port));
}
