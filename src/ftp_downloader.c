#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define MAX_THREADS 4
#define BUFFER_SIZE 1024

typedef struct {
    int thread_id;
    int data_fd;
    char *local_file;
    long long start_pos;
    long long end_pos;
} thread_data_t;

void send_ftp_command(int sock, const char *cmd) {
    write(sock, cmd, strlen(cmd));
    write(sock, "\r\n", 2);
}

void receive_ftp_response(int sock, char *response, size_t size) {
    ssize_t len = read(sock, response, size - 1);
    if (len > 0) {
        response[len] = '\0';
    } else {
        response[0] = '\0';
    }
}

int get_data_port(const char *response) {
    int h1, h2, h3, h4, p1, p2;
    sscanf(response, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)",
           &h1, &h2, &h3, &h4, &p1, &p2);
    return (p1 * 256) + p2;
}

void *download_thread(void *arg) {
    thread_data_t *data = (thread_data_t *)arg;
    char buffer[BUFFER_SIZE];
    int file_fd;
    long long bytes_to_read = data->end_pos - data->start_pos + 1;
    long long bytes_read = 0;

    // Open local file for writing
    file_fd = open(data->local_file, O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (file_fd < 0) {
        perror("Error opening file");
        pthread_exit(NULL);
    }

    // Seek to start position
    lseek(data->data_fd, data->start_pos, SEEK_SET);

    while (bytes_read < bytes_to_read) {
        int bytes = read(data->data_fd, buffer, BUFFER_SIZE);
        if (bytes <= 0) {
            break;
        }
        bytes_read += bytes;
        if (write(file_fd, buffer, bytes) < 0) {
            perror("Error writing to file");
            close(file_fd);
            pthread_exit(NULL);
        }
    }

    close(file_fd);
    close(data->data_fd);
    free(data);
    pthread_exit(NULL);
}

void download_file(const char *ftp_server, const char *ftp_user, const char *ftp_password, const char *remote_file, const char *local_file) {
    int control_fd, data_fd;
    struct sockaddr_in server_addr;
    struct hostent *server;
    char response[BUFFER_SIZE];

    // Connect to FTP server
    control_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (control_fd < 0) {
        perror("Error opening socket");
        return;
    }

    server = gethostbyname(ftp_server);
    if (server == NULL) {
        fprintf(stderr, "Error, no such host\n");
        return;
    }

    memset((char *) &server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    memcpy((char *) &server_addr.sin_addr.s_addr, (char *) server->h_addr, server->h_length);
    server_addr.sin_port = htons(21);

    if (connect(control_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("Error connecting to FTP server");
        return;
    }

    // Receive server response
    receive_ftp_response(control_fd, response, sizeof(response));
    printf("Server response: %s\n", response);

    // Send login commands
    send_ftp_command(control_fd, "USER ftpuser");
    receive_ftp_response(control_fd, response, sizeof(response));
    printf("Server response: %s\n", response);

    send_ftp_command(control_fd, "PASS password");
    receive_ftp_response(control_fd, response, sizeof(response));
    printf("Server response: %s\n", response);

    // Request passive mode
    send_ftp_command(control_fd, "PASV");
    receive_ftp_response(control_fd, response, sizeof(response));
    printf("Server response: %s\n", response);

    int data_port = get_data_port(response);

    // Connect to data port
    data_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (data_fd < 0) {
        perror("Error opening data socket");
        close(control_fd);
        return;
    }

    server_addr.sin_port = htons(data_port);

    if (connect(data_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("Error connecting to data port");
        close(control_fd);
        close(data_fd);
        return;
    }

    // Send RETR command to retrieve file
    char retr_cmd[BUFFER_SIZE];
    snprintf(retr_cmd, sizeof(retr_cmd), "RETR %s", remote_file);
    send_ftp_command(control_fd, retr_cmd);
    receive_ftp_response(control_fd, response, sizeof(response));
    printf("Server response: %s\n", response);

    // Download file using multiple threads
    pthread_t threads[MAX_THREADS];
    long long file_size = 102400; // Example size, you need to determine the actual file size
    long long part_size = file_size / MAX_THREADS;

    for (int i = 0; i < MAX_THREADS; ++i) {
        thread_data_t *data = (thread_data_t *)malloc(sizeof(thread_data_t));
        data->thread_id = i;
        data->data_fd = data_fd; // Use the same data connection
        data->local_file = strdup(local_file);
        data->start_pos = i * part_size;
        data->end_pos = (i == MAX_THREADS - 1) ? file_size - 1 : (i + 1) * part_size - 1;
        
        pthread_create(&threads[i], NULL, download_thread, (void *)data);
    }

    for (int i = 0; i < MAX_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    close(control_fd);
    printf("Download successfully! \n");
}
