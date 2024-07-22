import socket

def tcp_client(server_host='192.168.79.137', server_port=12346):
    # 创建 socket
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    print("TCP客户端Socket 已创建")

    try:
        # 连接服务器
        client_socket.connect((server_host, server_port))
        print(f"已连接到服务器 {server_host}:{server_port}")

        while True:
            # 输入要发送的数据
            message = input("请输入要发送给服务器的数据（输入 'exit' 退出）：")
            if message.lower() == 'exit':
                break

            # 发送数据到服务器
            client_socket.sendall(message.encode())
            print(f"已发送数据到服务器 {server_host}:{server_port}")

            # 接收来自服务器的响应
            data = client_socket.recv(1024)
            print(f"接收到来自服务器的数据：{data.decode()}")

    except KeyboardInterrupt:
        print("\n客户端已关闭")

    finally:
        # 关闭套接字
        client_socket.close()
        print("客户端Socket 已关闭")

if __name__ == "__main__":
    # 设置服务器的IP地址和端口
    tcp_client(server_host='192.168.79.137', server_port=12346)
