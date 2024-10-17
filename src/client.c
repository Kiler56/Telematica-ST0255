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
    struct sockaddr_in client_addr, server_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(server_addr);

    // Crear un socket UDP
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del cliente
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = INADDR_ANY;
    client_addr.sin_port = htons(DHCP_CLIENT_PORT);

    // Enlazar el socket a la dirección del cliente
    if (bind(sockfd, (const struct sockaddr *)&client_addr, sizeof(client_addr)) < 0) {
        perror("Error en el bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del servidor DHCP
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_BROADCAST; // Dirección de broadcast
    server_addr.sin_port = htons(DHCP_SERVER_PORT);

    // Habilitar envío de mensajes de broadcast
    int broadcastEnable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0) {
        perror("Error al habilitar broadcast");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Enviar mensaje DHCPDISCOVER (enviar datos dummy por ahora)
    strcpy(buffer, "DHCPDISCOVER");
    if (sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, addr_len) < 0) {
        perror("Error al enviar mensaje DHCPDISCOVER");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Mensaje DHCPDISCOVER enviado al servidor\n");

    // Recibir respuesta (DHCP Offer)
    int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server_addr, &addr_len);
    if (n < 0) {
        perror("Error al recibir datos");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    buffer[n] = '\0';
    printf("Respuesta recibida del servidor DHCP: %s\n", buffer);

    close(sockfd);
    return 0;
}
