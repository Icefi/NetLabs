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

int socket_fd;

void handle_shutdown(int sig);

int main(int argc, char **argv) {
    signal(SIGINT, handle_shutdown);



    int port;

    struct sockaddr_in serv_addr;

    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (socket_fd < 0) {
        perror("ERROR: Socket creation failed\n");
        exit(EXIT_FAILURE);
    }

    printf("Enter PORT:\n");
    scanf("%d", &port);

    char message[BUF_SIZE] = "";

    printf("Input i:\n");
    scanf("%s", message);

    int i = atoi(message);

    bzero((char*) &serv_addr, sizeof serv_addr);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (argc == 1)
        serv_addr.sin_addr.s_addr = INADDR_ANY;
    else
        inet_aton(argv[1], &serv_addr.sin_addr);

    char buffer[BUF_SIZE] = "";
    unsigned int msg_len, addr_len;

    for (int j = 0; j < 5; j++) {

        if (sendto(socket_fd, message, strlen(message), 0, (struct sockaddr*) &serv_addr, sizeof serv_addr) < 0) {
            printf("Error: Sending message failed\nTry again\n");
            exit(EXIT_FAILURE);
        }

        printf("Message %d sent!\n", j + 1);

        addr_len = sizeof serv_addr;
        if ((msg_len = recvfrom(socket_fd, buffer, BUF_SIZE, 0, (struct sockaddr*) &serv_addr, &addr_len)) < 0) {
            printf("Error: Sending message failed\nTry again\n");
            exit(EXIT_FAILURE);
        }

        printf("SERVER > %s\n", buffer);

        sleep(i);

    }

    close(socket_fd);

    return 0;
}

void handle_shutdown(int sig) {
    if(socket_fd) {
        close(socket_fd);
        printf("\nSocket closed\n");
    }

    exit(0);
}
