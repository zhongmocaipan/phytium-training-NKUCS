// main.c

#include <stdio.h>
#include <stdlib.h>
#include "ftp_downloader.h"
#include "file_transfer.h"

int main(int argc, char *argv[]) {
    if (argc < 6) {
        fprintf(stderr, "Usage: %s <ftp_server> <ftp_user> <ftp_password> <remote_file> <local_file>\n", argv[0]);
        return 1;
    }

    char *ftp_server = argv[1];
    char *ftp_user = argv[2];
    char *ftp_password = argv[3];
    char *remote_file = argv[4];
    char *local_file = argv[5];

    // Example FTP download
    download_file(ftp_server, ftp_user, ftp_password, remote_file, local_file);

    // Example file transfer (local and remote addresses are hardcoded for simplicity)
    // send_file("192.168.1.137", 21, local_file);
    // receive_file(21, "/final_project");

    return 0;
}
