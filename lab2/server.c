#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>

#define MULTY_THREAD_MODE 0
/*
 * 0 - by fork
 * 1 - by pthread
 * 2 - by select
 */

#define BUF_SIZE 1024

typedef struct TCPServer {
    int sockMain;

    struct sockaddr_in servAddr;
} TCPServer_t;

TCPServer_t TCP_SERVER;

int TCPServer_Init      ();
int TCPServer_Run       ();
int TCPServer_Terminate ();

void TCPServer_DisplayServerInfo();

void handle_shutdown(int sig) {
    TCPServer_Terminate();
    exit(0);
}

int main() {
    signal(SIGINT, handle_shutdown);

    int err;
    err = TCPServer_Init();

    switch (err) {
        case 1:
            printf("Error: Cannot create a socket\n");
            return 0;

        case 2:
            printf("Error: Cannot bind a socket\n");
            return 0;

        case 3:
            printf("Error: Cannot get socket name\n");
            return 0;
    }

    err = TCPServer_Run();

    switch (err) {
        case 1:
            printf("Error: Cannot reciev a message\n");
            return 0;

        case 2:
            printf("Error: Cannot fork a process\n");
            return 0;

        case 3:
            printf("Error: Cannot recieve a message\n");
            return 0;

        case 4:
            printf("Error: Cannot send a message\n");
            return 0;

    }

    err = TCPServer_Terminate();

    return 0;
}

int TCPServer_Init() {
    unsigned int addrLen;

    if ((TCP_SERVER.sockMain = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return 1;

    TCP_SERVER.servAddr.sin_family = AF_INET;
    TCP_SERVER.servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    TCP_SERVER.servAddr.sin_port = 0;

    if (bind(TCP_SERVER.sockMain, (struct sockaddr*) &TCP_SERVER.servAddr, sizeof TCP_SERVER.servAddr))
        return 2;

    addrLen = sizeof TCP_SERVER.servAddr;
    if (getsockname(TCP_SERVER.sockMain, (struct sockaddr*) &TCP_SERVER.servAddr, &addrLen))
        return 3;

    listen(TCP_SERVER.sockMain, 5);

    return 0;
}

int TCPServer_Run() {
    int sockClient, child;
    char buffer[BUF_SIZE];

    TCPServer_DisplayServerInfo();

    for(;;) {
        if ((sockClient = accept(TCP_SERVER.sockMain, 0, 0)) < 0)
            return 1;

        child = fork();

        if (child < 0)
            return 2;

        if (child == 0) {
            close(TCP_SERVER.sockMain);

            for (int i = 0; i < 10; i++) {

                sleep(atoi(buffer));

                if (recv(sockClient, buffer, BUF_SIZE, 0) < 0)
                    return 3;

                printf("CLIENT > %s\n\n", buffer);
                sprintf(buffer, "%d", 11 * atoi(buffer));

                if (send(sockClient, buffer, strlen(buffer), 0) < 0)
                    return 4;
            }

            close(sockClient);
            exit(0);
        }

        close(sockClient);
    }
}

int TCPServer_Terminate() {
    if(TCP_SERVER.sockMain) {
        close(TCP_SERVER.sockMain);
        printf("\nMain socket closed\n");
    }

    return 0;
}

void TCPServer_DisplayServerInfo() {
    printf("SERVER > %s : %d\n", inet_ntoa(TCP_SERVER.servAddr.sin_addr), ntohs(TCP_SERVER.servAddr.sin_port));
}
