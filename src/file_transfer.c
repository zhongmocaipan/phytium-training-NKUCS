// file_transfer.c

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

void send_file(const char *server_ip, int port, const char *file_path) {
    int sock_fd;
    struct sockaddr_in server_addr;
    FILE *file;
    char buffer[BUFFER_SIZE];

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("Error opening socket");
        return;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error connecting to server");
        return;
    }

    file = fopen(file_path, "rb");
    if (file == NULL) {
        perror("Error opening file");
        close(sock_fd);
        return;
    }

    while (!feof(file)) {
        size_t bytes_read = fread(buffer, 1, BUFFER_SIZE, file);
        if (bytes_read <= 0) {
            break;
        }
        write(sock_fd, buffer, bytes_read);
    }

    fclose(file);
    close(sock_fd);
}

void receive_file(int port, const char *save_path) {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    FILE *file;
    char buffer[BUFFER_SIZE];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Error opening socket");
        return;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error binding");
        close(server_fd);
        return;
    }

    listen(server_fd, 5);

    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
    if (client_fd < 0) {
        perror("Error accepting client connection");
        close(server_fd);
        return;
    }

    file = fopen(save_path, "wb");
    if (file == NULL) {
        perror("Error opening file");
        close(client_fd);
        close(server_fd);
        return;
    }

    while (1) {
        ssize_t bytes_read = read(client_fd, buffer, BUFFER_SIZE);
        if (bytes_read <= 0) {
            break;
        }
        fwrite(buffer, 1, bytes_read, file);
    }

    fclose(file);
    close(client_fd);
    close(server_fd);
}
