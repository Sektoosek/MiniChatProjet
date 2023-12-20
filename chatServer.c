#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

typedef struct {
    char ip[INET_ADDRSTRLEN];
    char username[50];
    int socket;
} UserInfo;

UserInfo users[MAX_CLIENTS];
int userCount = 0;
pthread_mutex_t userMutex = PTHREAD_MUTEX_INITIALIZER;

void handleConnection(int clientSocket, struct sockaddr_in clientAddr);
void handleMessage(char* message, char* ip, char* username);
void* clientThread(void* arg);

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientLen = sizeof(clientAddr);

    // Créer le socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Configurer l'adresse du serveur
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(55555);

    // Lier le socket à l'adresse et au port
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }

    // Mettre le serveur en mode écoute
    if (listen(serverSocket, 10) == -1) {
        perror("Error listening");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port 55555...\n");

    // Accepter les connexions et gérer chaque connexion dans un thread séparé
    while (1) {
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);

        if (clientSocket == -1) {
            perror("Error accepting connection");
            continue;
        }

        pthread_t thread;
        if (pthread_create(&thread, NULL, clientThread, &clientSocket) != 0) {
            perror("Error creating thread");
            close(clientSocket);
        }
    }

    close(serverSocket);

    return 0;
}

void* clientThread(void* arg) {
    int clientSocket = *(int*)arg;
    struct sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);

    if (getpeername(clientSocket, (struct sockaddr*)&clientAddr, &clientLen) == -1) {
        perror("Error getting client address");
        close(clientSocket);
        return NULL;
    }

    handleConnection(clientSocket, clientAddr);
    return NULL;
}

void handleConnection(int clientSocket, struct sockaddr_in clientAddr) {
    char username[50];
    recv(clientSocket, username, sizeof(username), 0);

    // Vérifier si le pseudonyme existe déjà
    pthread_mutex_lock(&userMutex);
    for (int i = 0; i < userCount; ++i) {
        if (strcmp(username, users[i].username) == 0) {
            // Pseudonyme déjà utilisé, informer le client et fermer la connexion
            pthread_mutex_unlock(&userMutex);
            char errorMessage[] = "Error: Pseudonyme déjà utilisé. Choisissez un autre pseudonyme.\n";
            send(clientSocket, errorMessage, sizeof(errorMessage), 0);
            close(clientSocket);
            return;
        }
    }

    // Ajouter l'utilisateur à la table
    UserInfo newUser;
    strcpy(newUser.ip, inet_ntoa(clientAddr.sin_addr));
    strcpy(newUser.username, username);
    newUser.socket = clientSocket;
    users[userCount++] = newUser;
    pthread_mutex_unlock(&userMutex);

    // Informer le client que la connexion est établie
    char welcomeMessage[100];
    sprintf(welcomeMessage, "Welcome, %s!", username);
    send(clientSocket, welcomeMessage, strlen(welcomeMessage), 0);

    // Boucle pour gérer les messages
    while (1) {
        char messageBuffer[BUFFER_SIZE];
        int bytesReceived = recv(clientSocket, messageBuffer, sizeof(messageBuffer), 0);

        if (bytesReceived <= 0) {
            // Gérer la déconnexion de l'utilisateur
            printf("User %s disconnected.\n", username);

            // Retirer l'utilisateur de la table
            pthread_mutex_lock(&userMutex);
            for (int i = 0; i < userCount; ++i) {
                if (strcmp(username, users[i].username) == 0) {
                    // Décaler les éléments suivants pour remplir l'espace
                    for (int j = i; j < userCount - 1; ++j) {
                        users[j] = users[j + 1];
                    }
                    userCount--;
                    break;
                }
            }
            pthread_mutex_unlock(&userMutex);

            close(clientSocket);
            break;
        }

        handleMessage(messageBuffer, newUser.ip, newUser.username);
    }
}

void handleMessage(char* message, char* ip, char* username) {
    time_t currentTime;
    struct tm* timeInfo;
    char timeString[9];

    time(&currentTime);
    timeInfo = localtime(&currentTime);

    strftime(timeString, sizeof(timeString), "%H:%M:%S", timeInfo);

    // Format du message : HH-mm-ss — IP Adresse - Nom Utilisateur - Message
    char formattedMessage[BUFFER_SIZE + 30];
    sprintf(formattedMessage, "%s — %s - %s - %s", timeString, ip, username, message);

    // Envoyer le message à tous les utilisateurs connectés
    pthread_mutex_lock(&userMutex);
    for (int i = 0; i < userCount; ++i) {
        send(users[i].socket, formattedMessage, strlen(formattedMessage), 0);
    }
    pthread_mutex_unlock(&userMutex);

    // Enregistrer le message dans le fichier chat.txt
    int fileDescriptor = open("chat.txt", O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
    write(fileDescriptor, formattedMessage, strlen(formattedMessage));
    close(fileDescriptor);

    // Afficher le message dans le terminal du serveur
    printf("%s\n", formattedMessage);
}

