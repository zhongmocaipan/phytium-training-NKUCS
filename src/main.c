#include <stdio.h>
#include <stdlib.h>
#include "ftp_downloader.h"
#include "file_transfer.h"

int main(int argc, char *argv[]) {
    if (argc < 7) {
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "  %s 1 <ftp_server> <ftp_user> <ftp_password> <remote_file> <local_file>  // For downloading\n", argv[0]);
        fprintf(stderr, "  %s 2 <local_address> <local_port> <remote_file> <remote_address> <remote_port>  // For file transfer\n", argv[0]);
        return 1;
    }

    int option = atoi(argv[1]);
    if (option == 1) {
        // Download tool
        if (argc != 7) {
            fprintf(stderr, "Usage: %s 1 <ftp_server> <ftp_user> <ftp_password> <remote_file> <local_file>\n", argv[0]);
            return 1;
        }

        char *ftp_server = argv[2];
        char *ftp_user = argv[3];
        char *ftp_password = argv[4];
        char *remote_file = argv[5];
        char *local_file = argv[6];

        download_file(ftp_server, ftp_user, ftp_password, remote_file, local_file);
    } else if (option == 2) {
        // File transfer tool
        if (argc != 7) {
            fprintf(stderr, "Usage: %s 2 <local_address> <local_port> <remote_file> <remote_address> <remote_port>\n", argv[0]);
            return 1;
        }

        char *local_address = argv[2];
        int local_port = atoi(argv[3]);
        char *remote_file = argv[4];
        char *remote_address = argv[5];
        int remote_port = atoi(argv[6]);

        // Replace with actual file transfer function calls
        send_file(local_address, local_port, remote_file);
        receive_file(remote_port, remote_address);
    } else {
        fprintf(stderr, "Invalid option. Use 1 for download or 2 for file transfer.\n");
        return 1;
    }

    return 0;
}
