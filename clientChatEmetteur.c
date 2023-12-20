#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MESSAGE_SIZE 256

void *receive_messages(void *arg);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_ip> <server_port>\n", argv[0]);
        exit(1);
    }

    char *server_ip = argv[1];
    int server_port = atoi(argv[2]);

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    inet_pton(AF_INET, server_ip, &(server_addr.sin_addr));
    memset(server_addr.sin_zero, '\0', sizeof(server_addr.sin_zero));

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        exit(1);
    }

    printf("Connected to server %s:%d\n", server_ip, server_port);

    char username[20];
    printf("Enter your username: ");
    fgets(username, sizeof(username), stdin);
    username[strlen(username) - 1] = '\0'; // Remove newline character

    // Send the username to the server
    if (send(client_socket, username, strlen(username), 0) == -1) {
        perror("send");
        close(client_socket);
        exit(1);
    }

    // Create a thread to read messages from the server and display them
    pthread_t receive_thread;
    if (pthread_create(&receive_thread, NULL, receive_messages, &client_socket) != 0) {
        perror("pthread_create");
        close(client_socket);
        exit(1);
    }

    char message[MESSAGE_SIZE];
    while (1) {
        fgets(message, sizeof(message), stdin);
        if (send(client_socket, message, strlen(message), 0) == -1) {
            perror("send");
            break;
        }
    }

    // Close the client socket
    close(client_socket);

    return 0;
}

void *receive_messages(void *arg) {
    int client_socket = *((int *)arg);
    char message[MESSAGE_SIZE];

    while (1) {
        int bytes_received = recv(client_socket, message, sizeof(message), 0);
        if (bytes_received <= 0) {
            break;
        }

        message[bytes_received] = '\0';
        printf("%s\n", message);
    }

    pthread_exit(NULL);
}


