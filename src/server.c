#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>

#define DHCP_CLIENT_PORT 70
#define DHCP_SERVER_PORT 69
#define BUFFER_SIZE 512
#define IP_POOL_SIZE 10  // Tamaño del pool de direcciones IP

// Rango de direcciones IP a gestionar
const char *ip_pool[IP_POOL_SIZE] = {
    "192.168.1.100",
    "192.168.1.101",
    "192.168.1.102",
    "192.168.1.103",
    "192.168.1.104",
    "192.168.1.105",
    "192.168.1.106",
    "192.168.1.107",
    "192.168.1.108",
    "192.168.1.109"
};

int ip_assigned[IP_POOL_SIZE] = {0};  // Marcar IPs como asignadas

// Función para obtener una IP disponible del pool
const char *get_available_ip() {
    for (int i = 0; i < IP_POOL_SIZE; i++) {
        if (ip_assigned[i] == 0) {  // Si la IP no está asignada
            ip_assigned[i] = 1;
            return ip_pool[i];
        }
    }
    return NULL;  // No hay IPs disponibles
}

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

        buffer[n] = '\0';  // Asegurar que el buffer es un string válido
        printf("Solicitud recibida de %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Procesar el mensaje DHCP DISCOVER
        if (strcmp(buffer, "DHCPDISCOVER") == 0) {
            const char *available_ip = get_available_ip();
            if (available_ip != NULL) {
                printf("Asignando IP: %s\n", available_ip);

                // Responder con un mensaje DHCPOFFER
                snprintf(buffer, BUFFER_SIZE, "DHCPOFFER %s", available_ip);
                if (sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&client_addr, addr_len) < 0) {
                    perror("Error al enviar mensaje DHCPOFFER");
                } else {
                    printf("Mensaje DHCPOFFER enviado: %s\n", buffer);
                }
            } else {
                printf("No hay más IPs disponibles\n");
            }
        }

        // Aquí puedes agregar el procesamiento de otros mensajes DHCP como REQUEST y ACK
    }

    close(sockfd);
    return 0;
}
