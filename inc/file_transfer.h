// file_transfer.h

#ifndef FILE_TRANSFER_H
#define FILE_TRANSFER_H

void send_file(const char *server_ip, int port, const char *file_path);
void receive_file(int port, const char *save_path);

#endif
