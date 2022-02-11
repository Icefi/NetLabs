#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define MAX_LEN 1024

int main() {
    int sockfd;
    int port;

    //char message[MAX_LEN];
    //char ip_str [16];

    struct sockaddr_in serv_addr;
    //struct in_addr ip_adr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0) {
        perror("ERROR: Socket creation failed\n");
        exit(EXIT_FAILURE);
    }

    printf("Enter PORT > ");
    scanf("%d", &port);

    char message[MAX_LEN];

    printf("Input message:\n");
    //fgets(message, MAX_LEN, stdin);
    scanf("%s", message);

    printf("CLIENT: Message size is %ld\n", strlen(message));

    bzero((char*) &serv_addr, sizeof serv_addr);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    //serv_addr.sin_addr = ip_adr;
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    if (sendto(sockfd, message, strlen(message), 0, (struct sockaddr*) &serv_addr, sizeof serv_addr) < 0) {
        printf("Error: Sending message failed\nTry again\n");
        exit(EXIT_FAILURE);
    }

    printf("Message sent!\n");

    close(sockfd);

    return 0;
}
