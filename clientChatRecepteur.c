#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#define FIFO_NAME "message_fifo"

int main() {
    int message_fifo;
    char message[256];

    // Création du tube nommé
    if (mkfifo(FIFO_NAME, 0666) == -1) {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }

    message_fifo = open(FIFO_NAME, O_WRONLY);
    if (message_fifo == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    while (1) {
        printf("Enter a message: ");
        fgets(message, sizeof(message), stdin);

        // Écriture du message dans le tube nommé
        if (write(message_fifo, message, strlen(message)) == -1) {
            perror("write");
            break;
        }
    }

    close(message_fifo);
    unlink(FIFO_NAME); // Suppression du tube nommé après utilisation


    return 0;
}

