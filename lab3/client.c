// Клиентская реализация модели клиент-сервер UDP
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAXLINE 1024

int main() {

    int sockfd;

    char msg[MAXLINE] = "r";
    char buffer[MAXLINE];

    struct sockaddr_in servaddr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {

    perror("socket creation failed");

    exit(EXIT_FAILURE);
    }

    servaddr.sin_family = AF_INET;
    printf("Write port number: ");
    int port;
    scanf("%d", &port);
    servaddr.sin_port = htons(port);
    printf("Write IP addr: ");
    char ipcli[20];
    scanf("%s", ipcli);
    servaddr.sin_addr.s_addr = inet_addr(ipcli);
    printf("Write i: ");
    scanf("%s", msg);

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof servaddr)) {
        perror("Conncet er");
        exit(1);
    }

    int i = atoi(msg);

    unsigned int n = 0;

    for (int j = 0; j < 10; j++) {
        sleep(i);
        if (send(sockfd, (const char *)msg, strlen(msg), 0) < 0)
            printf("Send err\n");

        n = recv(sockfd, (char *)buffer, MAXLINE, 0);

        if (n < 0)
            printf("Recv err\n");

        buffer[n] = '\0';

        printf("SERVER: %s\n", buffer);
    }

    close(sockfd);

    return 0;
}
