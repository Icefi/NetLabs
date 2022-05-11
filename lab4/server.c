#include "tcpserver.h"

#include <signal.h>

TCPServer_t server;

void handle_shutdown(int sig) {
    TCPServer_Terminate(&server);
    exit(EXIT_SUCCESS);
}

void on_start(TCPServer_t* self) {
    TCPServer_DisplayServerInfo(self);
}

void on_recieve(TCPServer_t* self, char* buf, int sockfd) {
    printf("CLIENT[%d] > %s\n", sockfd, buf);

    int temp = atoi(buf) * 10;
    sprintf(buf, "%d", temp);
}

int main() {
    signal(SIGINT, handle_shutdown);

    switch (TCPServer_Init(&server)) {
        case 1:
            printf("Error: Cannot initialise new server\n");
            return 0;

        case 2:
            printf("Error: Cannot create a socket\n");
            return 0;

        case 3:
            printf("Error: Cannot bind a socket\n");
            return 0;

        case 4:
            printf("Error: Cannot get socket name\n");
            return 0;
    }

    server.on_start = on_start;
    server.on_recv  = on_recieve;

    switch (TCPServer_Run(&server)) {
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

    TCPServer_Terminate(&server);

    return 0;
}
