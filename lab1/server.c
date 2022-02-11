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
    int sockfd, port, len, n;

    char buffer[MAX_LEN];
    char *hello = "Hello from server";

    struct sockaddr_in serv_addr, cli_addr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    bzero((char*) &serv_addr, sizeof serv_addr);

    serv_addr.sin_family      = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port        = 0;



    bind (sockfd, (const struct sockaddr*) &serv_addr, sizeof serv_addr);

    len = sizeof serv_addr;

    getsockname (sockfd, (struct sockaddr*) &serv_addr, &len);

    printf("SERVER: IP   is %s\n", inet_ntoa(serv_addr.sin_addr));
    printf("SERVER: Port is %d\n", ntohs(serv_addr.sin_port));

    len = sizeof cli_addr;

    n = recvfrom(sockfd, (char *)buffer, MAX_LEN, MSG_WAITALL, (struct sockaddr*) &cli_addr, &len);

    buffer[n] = '\0';

    printf("Client : %s\n", buffer);
    sendto(sockfd, (const char *)hello, strlen(hello),
        MSG_CONFIRM, (const struct sockaddr *) &cli_addr,
            len);
    printf("Hello message sent.\n");

    return 0;
}
