#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PORT 0
#define MAXLINE 1024
int clientSocket;
void handle_shutdown(int sig)
{
    if(clientSocket)
    {
        close(clientSocket);
        printf("\nClient socket closed\n");
    }
    exit(0);
    
}

int main() {
    signal(SIGINT, handle_shutdown);
    int i, j;

    char msg[MAXLINE] = "r";
    char buffer[MAXLINE];
    struct sockaddr_in client;

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(clientSocket < 0) {
		perror("Error in connection.\n");
		exit(1);
	}
	printf("Client Socket is created.\n");

    client.sin_family = AF_INET;
    printf("Write port number: ");
    int port;
    scanf("%d", &port);
    client.sin_port = htons(port);
    client.sin_addr.s_addr = inet_addr("127.0.0.1");
    printf("Write i: ");
    scanf("%s", msg);

    i = atoi(msg);
    j = 0;
    unsigned int n = 0, len = 0;

    int ret = connect(clientSocket, (struct sockaddr*)&client, sizeof(client));
    if(ret < 0) {
        perror("Error in connection.\n");
        exit(1);
    }
    printf("Connected to the Server.\n");
    while (j < 100) {
        sleep(i);
        if (send(clientSocket, (const char *)msg, strlen(msg), 0) < 0)
            printf("Send err\n");
        n = recv(clientSocket, (char *)buffer, MAXLINE, 0);
        if (n <= 0)
        {
            printf("Client shutdown\n");
            close(clientSocket);
            exit(1);
        }
        buffer[n] = '\0';

        printf("SERVER: %s\n", buffer);
        j++;
    }
    close(clientSocket);

    return 0;
}
