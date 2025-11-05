#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];
    fd_set readfds;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Socket creation error\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Connect to localhost
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("Invalid address\n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connection Failed\n");
        return -1;
    }

    printf("Connected to chat server. Type messages and press Enter.\n");

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(0, &readfds);     // stdin
        FD_SET(sock, &readfds);  // socket

        int max_sd = sock;
        select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if (FD_ISSET(0, &readfds)) {
            // Send user input
            fgets(buffer, BUFFER_SIZE, stdin);
            send(sock, buffer, strlen(buffer), 0);
        }

        if (FD_ISSET(sock, &readfds)) {
            // Receive message
            int valread = read(sock, buffer, BUFFER_SIZE);
            if (valread <= 0) {
                printf("Disconnected from server.\n");
                break;
            }
            buffer[valread] = '\0';
            printf(">> %s", buffer);
        }
    }

    close(sock);
    return 0;
}
