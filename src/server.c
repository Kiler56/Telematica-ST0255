#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define DHCP_CLIENT_PORT 70
#define DHCP_SERVER_PORT 69
#define BUFFER_SIZE 512

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(client_addr);

    // Crear un socket UDP
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del servidor
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(DHCP_SERVER_PORT);

    // Enlazar el socket con la dirección IP y puerto
    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error en el bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Servidor DHCP escuchando en el puerto %d...\n", DHCP_SERVER_PORT);

    // Bucle para escuchar mensajes DHCP
    while (1) {
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
        if (n < 0) {
            perror("Error al recibir datos");
            continue;
        }

        buffer[n] = '\0'; // Asegurar que el buffer es un string válido
        printf("Solicitud recibida de %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        
        // Aquí es donde se procesarían los mensajes DHCP
    }

    close(sockfd);
    return 0;
}
