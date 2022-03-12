#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <pthread.h>

#define BUF_SIZE 1024
#define CLIENT_MAX 20

typedef struct TCPServer {
    int sockMain;
    int sockArr[CLIENT_MAX];
    int sockArrSize;

    FILE* file;

    struct sockaddr_in servAddr;
} TCPServer_t;

TCPServer_t TCP_SERVER;

int TCPServer_Init      (const char*);
int TCPServer_Run       (void* (*)(void*));
int TCPServer_Terminate ();

void TCPServer_DisplayServerInfo();

void handle_shutdown(int sig) {
    TCPServer_Terminate();
    exit(0);
}

void *buff_work(void *input) {

    char buf[BUF_SIZE] = "";

    int msgLength;
    int sockClient = *(int*)input;

    for(int i = 0; i < 20; i++) {
        msgLength = recv(sockClient, buf, BUF_SIZE, 0);

        if (msgLength < 0) {
            close(sockClient);
            perror("Error: cannot recieve a message");
            exit(1);
        }

        if (msgLength == 0) {
            close(sockClient);
            printf("SERVER > Socket had been closed\n");
            break;
        }

        fwrite(buf, msgLength, 1, TCP_SERVER.file);

        send(sockClient, buf, msgLength, 0);

        printf("CLIENT [%d] > %s\n\n", sockClient, buf);
        sleep(atoi(buf));
    }
    close(sockClient);
    printf("Socket [%d] closed\n", sockClient);

    return 0;
}

int main() {
    signal(SIGINT, handle_shutdown);

    switch (TCPServer_Init("log.txt")) {
        case 1:
            printf("Error: Cannot create a socket\n");
            return -1;

        case 2:
            printf("Error: Cannot bind a socket\n");
            return -1;

        case 3:
            printf("Error: Cannot get socket name\n");
            return -1;

        case 4:
            printf("Error: Cannot open the file\n");
            return -1;

        default:
            printf("TCP Server inited!\n");
            TCPServer_DisplayServerInfo();
            printf("\n");
    }

    switch (TCPServer_Run(buff_work)) {
        case 1:
            printf("Error: Cannot reciev a message\n");
            return -1;

        case 2:
            printf("Error: Cannot create a new thread\n");
            return -1;

    }

    TCPServer_Terminate();

    return 0;
}

int TCPServer_Init(const char* file) {
    unsigned int addrLen;

    if ((TCP_SERVER.sockMain = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return 1;

    TCP_SERVER.sockArrSize = 0;
    TCP_SERVER.servAddr.sin_family = AF_INET;
    TCP_SERVER.servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    TCP_SERVER.servAddr.sin_port = 0;

    if (bind(TCP_SERVER.sockMain, (struct sockaddr*) &TCP_SERVER.servAddr, sizeof TCP_SERVER.servAddr))
        return 2;

    addrLen = sizeof TCP_SERVER.servAddr;
    if (getsockname(TCP_SERVER.sockMain, (struct sockaddr*) &TCP_SERVER.servAddr, &addrLen))
        return 3;

    listen(TCP_SERVER.sockMain, 5);

    TCP_SERVER.file = fopen(file, "a");
    if (TCP_SERVER.file == NULL)
        return 4;

    return 0;
}

int TCPServer_Run(void* (*buff_work)(void*)) {
    int sockClient;
    int status = 0, status_addr = 0;

    pthread_t child;

    for(;;) {
        if ((sockClient = accept(TCP_SERVER.sockMain, 0, 0)) < 0)
            return 1;

        status = pthread_create(&child, NULL, buff_work, &sockClient);

        if (status != 0)
            return 2;

        TCP_SERVER.sockArr[TCP_SERVER.sockArrSize] = sockClient;
        TCP_SERVER.sockArrSize++;

        if(TCP_SERVER.sockArrSize == CLIENT_MAX)
            break;
    }

    status = pthread_join(child, (void**)&status_addr);

    printf("SERVER > connection exceeded\n");
    fclose(TCP_SERVER.file);
    close(TCP_SERVER.sockMain);

    return 0;
}

int TCPServer_Terminate() {
    for(int i = 0; i < TCP_SERVER.sockArrSize; i++) {
        close(TCP_SERVER.sockArr[i]);
        printf("SERVER > Closed %d: %d\n", i, TCP_SERVER.sockArr[i]);
    }

    close(TCP_SERVER.sockMain);

    printf("\nFile closed\n");
    fclose(TCP_SERVER.file);

    return 0;
}

void TCPServer_DisplayServerInfo() {
    printf("SERVER > %s : %d\n", inet_ntoa(TCP_SERVER.servAddr.sin_addr), ntohs(TCP_SERVER.servAddr.sin_port));
}
