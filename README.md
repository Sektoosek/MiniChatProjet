# MiniChatProjet

# Documentation pour Application de Chat Client-Serveur (Client)

Ce document fournit une documentation pour une application de chat client-serveur simple écrite en C. L'application facilite la communication entre un client et un serveur via une connexion TCP/IP. Le client et le serveur communiquent à l'aide de sockets, et l'utilisateur interagit avec le client via une interface console.

# Utilisation

Pour utiliser l'application de chat, les arguments de ligne de commande suivants sont nécessaires : 

./client <server_ip> <server_port>
•	<server_ip> : L'adresse IP du serveur.
•	<server_port> : Le numéro de port sur lequel le serveur écoute.

Exemple :
./client 127.0.0.1 8080

# Implémentation

Dépendances
•	Bibliothèques standard C : stdio.h, stdlib.h, string.h, unistd.h
•	Bibliothèques réseau : arpa/inet.h
•	Bibliothèques de threads : pthread.h
•	Bibliothèques de contrôle de fichiers : fcntl.h
•	Bibliothèques d'appels système : sys/types.h, sys/stat.h

# Constantes

BUFFER_SIZE : Définit la taille du tampon de message pour l'envoi et la réception de messages.

# Fonctions

main
La fonction principale est le point d'entrée de l'application client. Elle effectue les tâches suivantes :
1.	Analyse les arguments de ligne de commande pour obtenir l'adresse IP et le port du serveur.
2.	Crée un socket pour la communication avec le serveur.
3.	Configure l'adresse du serveur en utilisant struct sockaddr_in.
4.	Se connecte au serveur.
5.	Invite l'utilisateur à saisir un nom d'utilisateur et l'envoie au serveur.
6.	Crée deux threads :
o	userInputThread : Gère la saisie utilisateur et envoie des messages au serveur.
o	serverListenerThread : Écoute les messages du serveur et les affiche.
userInputThread
Ce thread est responsable de la capture de la saisie utilisateur et de l'envoi de messages au serveur. Il s'exécute dans une boucle infinie, invitant l'utilisateur à saisir des messages et les envoyant au serveur.
serverListenerThread
Ce thread écoute les messages du serveur. Il s'exécute dans une boucle infinie, recevant des messages et les affichant. S'il y a une déconnexion du serveur, il avertit l'utilisateur et se termine.

# Interaction avec l'Utilisateur

•	Après la connexion au serveur, l'utilisateur est invité à saisir un nom d'utilisateur.
•	L'utilisateur peut ensuite saisir des messages dans la console, qui sont envoyés au serveur.
•	Les messages reçus du serveur sont affichés dans la console.

# Gestion des Erreurs

L'application fournit des messages d'erreur et se termine correctement en cas d'échecs, tels que la création du socket ou les erreurs de connexion.

# Nettoyage

L'application ferme le socket avant de se terminer.

# Documentation pour Application de Chat Client-Serveur (Serveur)

Ce document fournit une documentation pour la partie serveur d'une application de chat client-serveur écrite en C. L'application permet à plusieurs clients de se connecter simultanément et d'échanger des messages à travers le serveur. Chaque client est identifié par un pseudonyme unique. Le serveur gère les connexions, les déconnexions, et la diffusion des messages à tous les clients connectés.

# Implémentation
Dépendances
•	Bibliothèques standard C : stdio.h, stdlib.h, string.h, unistd.h
•	Bibliothèques réseau : arpa/inet.h
•	Bibliothèques de threads : pthread.h
•	Bibliothèques de gestion de fichiers : fcntl.h
•	Bibliothèques d'appels système : sys/types.h, sys/stat.h
•	Bibliothèques de gestion du temps : time.h

# Constantes
•	MAX_CLIENTS : Définit le nombre maximal de clients que le serveur peut gérer simultanément.
•	BUFFER_SIZE : Définit la taille du tampon de message pour l'envoi et la réception de messages.
Structures de Données
•	UserInfo : Structure contenant les informations sur un utilisateur (adresse IP, pseudonyme, socket).
Variables Globales
•	users : Tableau de structures UserInfo contenant les informations sur tous les utilisateurs connectés.
•	userCount : Variable indiquant le nombre actuel d'utilisateurs connectés.
•	userMutex : Mutex pour assurer une manipulation sûre de la table des utilisateurs par plusieurs threads.

# Fonctions
main
La fonction principale du serveur gère la création du socket, la configuration de l'adresse du serveur, la liaison du socket à l'adresse et au port, la mise en mode écoute, et l'acceptation des connexions entrantes. Chaque nouvelle connexion est gérée par un thread distinct (clientThread).
clientThread
Ce thread est responsable de la gestion d'une connexion client individuelle. Il appelle la fonction handleConnection pour gérer l'établissement initial de la connexion et ensuite gère la réception des messages.
handleConnection
Cette fonction est appelée lorsqu'un client établit une connexion. Elle vérifie la disponibilité du pseudonyme du client, l'ajoute à la table des utilisateurs, envoie un message de bienvenue au client, et gère les messages entrants jusqu'à la déconnexion du client.
handleMessage
Cette fonction est appelée pour traiter les messages reçus d'un client. Elle formate le message avec l'heure, l'adresse IP, le pseudonyme du client, et le message lui-même. Ensuite, elle envoie le message à tous les clients connectés, l'enregistre dans le fichier chat.txt, et l'affiche dans le terminal du serveur.

# Gestion des Erreurs

•	Le serveur fournit des messages d'erreur en cas de problèmes lors de la création du socket, la liaison, la mise en mode écoute, et la gestion des connexions.

 # Nettoyage
•	Le serveur libère les ressources en fermant le socket avant de se terminer.




