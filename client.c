#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BUFFER_SIZE 1024

void* userInputThread(void* arg);
void* serverListenerThread(void* arg);

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_ip> <server_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char serverIP[15];
    strcpy(serverIP, argv[1]);
    int serverPort = atoi(argv[2]);

    // Créer le socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Configurer l'adresse du serveur
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    if (inet_pton(AF_INET, serverIP, &serverAddr.sin_addr) <= 0) {
        perror("Invalid address");
        exit(EXIT_FAILURE);
    }

    // Connecter le client au serveur
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error connecting to server");
        exit(EXIT_FAILURE);
    }

    printf("Connected to the server.\n");

    // Récupérer le pseudonyme du client
    char username[50];
    printf("Enter your username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0'; // Supprimer le saut de ligne

    // Envoyer le pseudonyme au serveur
    send(clientSocket, username, strlen(username), 0);

    // Créer des threads pour gérer la saisie utilisateur et la réception des messages
    pthread_t inputThread, listenerThread;
    pthread_create(&inputThread, NULL, userInputThread, (void*)&clientSocket);
    pthread_create(&listenerThread, NULL, serverListenerThread, (void*)&clientSocket);

    // Attendre que les threads se terminent
    pthread_join(inputThread, NULL);
    pthread_join(listenerThread, NULL);

    // Fermer le socket
    close(clientSocket);

    return 0;
}

void* userInputThread(void* arg) {
    int clientSocket = *(int*)arg;
    char message[BUFFER_SIZE];
    char tempBuffer[BUFFER_SIZE];
    int tempBufferLength = 0;

    while (1) {
        printf("Enter message: %s", tempBuffer); // Afficher le buffer temporaire
        fflush(stdout);

        if (fgets(message, BUFFER_SIZE, stdin) != NULL) {
            // Concaténer le buffer temporaire et le message
            strncat(tempBuffer, message, BUFFER_SIZE - tempBufferLength - 1);
            tempBufferLength = strlen(tempBuffer);

            // Vérifier si le message contient un retour à la ligne
            if (strchr(tempBuffer, '\n')) {
                tempBuffer[strcspn(tempBuffer, "\n")] = 0; // Supprimer le saut de ligne
                send(clientSocket, tempBuffer, strlen(tempBuffer), 0);

                // Réinitialiser le buffer temporaire
                memset(tempBuffer, 0, BUFFER_SIZE);
                tempBufferLength = 0;
            }
        }
    }

    return NULL;
}




void* serverListenerThread(void* arg) {
    int clientSocket = *(int*)arg;
    char receivedMessage[BUFFER_SIZE];

    while (1) {
        memset(receivedMessage, 0, BUFFER_SIZE);

        int bytesReceived = recv(clientSocket, receivedMessage, BUFFER_SIZE, 0);
        if (bytesReceived > 0) {
            receivedMessage[bytesReceived] = '\0';
            
            // Traiter seulement les messages qui ne sont pas des messages systèmes
            if (strstr(receivedMessage, "Welcome") == NULL) {
                printf("\n%s\n", receivedMessage);
                printf("Enter message: ");
                fflush(stdout);
            }
        } else if (bytesReceived <= 0) {
            break;
        }
    }

    return NULL;
}










