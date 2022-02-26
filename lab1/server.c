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

typedef struct UDPServer {
    int socket_fd;

    struct sockaddr_in servAddr;
    struct sockaddr_in cliAddr;

} UDPServer_t;

UDPServer_t UDP_SERVER;

int UDPServer_Init      ();
int UDPServer_Run       ();
int UDPServer_Terminate ();

void UDPServer_DisplayServerInfo();
void UDPServer_DisplayClientInfo();

void handle_shutdown(int sig) {
    UDPServer_Terminate();
    exit(0);
}

int main() {
    signal(SIGINT, handle_shutdown);

    int err;
    err = UDPServer_Init();

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

    err = UDPServer_Run();

    switch (err) {
        case 1:
            printf("Error: Cannot reciev a message\n");
            return 0;

        case 2:
            printf("Error: Sending message failed\n");
            return 0;

    }

    err = UDPServer_Terminate();

    return 0;
}

int UDPServer_Init() {
    if ((UDP_SERVER.socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        return 1;

    UDP_SERVER.servAddr.sin_family = AF_INET;
    UDP_SERVER.servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    UDP_SERVER.servAddr.sin_port = 0;

    if (bind(UDP_SERVER.socket_fd, (struct sockaddr*) &(UDP_SERVER.servAddr), sizeof UDP_SERVER.servAddr))
        return 2;

    unsigned int addrLen = sizeof UDP_SERVER.servAddr;
    if (getsockname(UDP_SERVER.socket_fd, (struct sockaddr*) &(UDP_SERVER.servAddr), &addrLen))
        return 3;

    return 0;
}

int UDPServer_Run() {
    char buffer[BUF_SIZE];

    UDPServer_DisplayServerInfo();

    unsigned int msgLen, addrLen;

    for (;;) {
        addrLen = sizeof UDP_SERVER.cliAddr;

        printf("\nWaiting...\n");

        bzero(buffer, BUF_SIZE);

        if ((msgLen = recvfrom(UDP_SERVER.socket_fd, buffer, BUF_SIZE, 0, (struct sockaddr*) &UDP_SERVER.cliAddr, &addrLen)) < 0)
            return 1;

        UDPServer_DisplayClientInfo();

        buffer[1] = '0';
        buffer[2] = '\0';

        printf("CLIENT > \'%s\'\n", buffer);

        if (sendto(UDP_SERVER.socket_fd, buffer, strlen(buffer), 0, (struct sockaddr*) &UDP_SERVER.cliAddr, sizeof UDP_SERVER.servAddr) < 0)
            return 2;
    }
}

int UDPServer_Terminate() {
    if(UDP_SERVER.socket_fd) {
        close(UDP_SERVER.socket_fd);
        printf("\nSocket closed\n");
    }

    return 0;
}

void UDPServer_DisplayServerInfo() {
    printf("SERVER > %s : %d\n", inet_ntoa(UDP_SERVER.servAddr.sin_addr), ntohs(UDP_SERVER.servAddr.sin_port));
}

void UDPServer_DisplayClientInfo() {
    printf("CLIENT > %s : %d\n", inet_ntoa(UDP_SERVER.cliAddr.sin_addr), ntohs(UDP_SERVER.cliAddr.sin_port));
}
