#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

#define DEFAULT_PORT 46645
#define LISTEN_BACKLOG 5

void* handleConnection(int *a_client) {
    char buffer[1024];
    int bytes_read;
    
    int client_fd = *((int*)a_client);

    while((bytes_read = read(client_fd, buffer, sizeof(buffer))) > 0) {
        buffer[bytes_read] = '\0'; // Null terminate the buffer

        printf("Received: %s\n", buffer);
        // write(a_client, buffer, bytes_read);
    }

    if(bytes_read == 0) {
        printf("Client Disconnected\n");
    }

    close(client_fd); // Close connection when finished

    return NULL;
}


int main(int argc, char* argv[]) {
    int port = DEFAULT_PORT;
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in socket_address;
    memset(&socket_address, '\0', sizeof(socket_address));
    socket_address.sin_family = AF_INET;
    socket_address.sin_addr.s_addr = htonl(INADDR_ANY);
    

    if(strcmp(argv[1], "-p") == 0) {
        port = atoi(argv[2]);
        socket_address.sin_port = htons(port);
    } else {
        socket_address.sin_port = htons(DEFAULT_PORT);
    }

    int returnval;

    returnval = bind(
        socket_fd, (struct sockaddr*)&socket_address, sizeof(socket_address));
    
    returnval = listen(socket_fd, LISTEN_BACKLOG);

    while(1) {
        struct sockaddr_in client_address;
        socklen_t client_address_len = sizeof(client_address);

        int client_fd = accept(
            socket_fd, (struct sockaddr*)&client_address, &client_address_len);
        handleConnection((void*)&client_fd);

        pthread_t connection_thread;
        if(pthread_create(&connection_thread, NULL, (void*)handleConnection, (void*)&client_fd)) {
            printf("Failed to create thread");
            close(client_fd);
        } else {
            pthread_join(connection_thread, NULL);
        }
    }


    close(socket_fd);

    return 0;
}