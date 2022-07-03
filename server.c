#include <pthread.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdint.h>

#define PORT_NUM 4000
#define LISTEN_BACKLOG 100  // so luong request toi da 1 server nhan

// luu thong tin nguoi dung khi dang nhap vao server




typedef struct ChatUser
{
    int fd;
    char chat_name[100];
} ChatUser;

ChatUser register_list[100];  // noi nguoi dung duoc luu: database
int register_size = 0;

void add_service(int sockfd, char *chat_name) // add nguoi dung
{
    memset(register_list[register_size].chat_name, 0, sizeof(register_list[register_size].chat_name));
    register_list[register_size].fd = sockfd;
    strcpy(register_list[register_size].chat_name, chat_name);
    register_size++;
}

void remove_service(int sockfd) // xoa nguoi dung
{
    for (int i = 0; i < register_size; i++)  // doc list, xem ai giong voi sockfd thi xoa
    {
        if (register_list[i].fd == sockfd)
        {
            for (int j = i; j < register_size - 1; j++)
            {
                register_list[j] = register_list[j + 1];
            }
            break;
        }
    }
    register_size--;
}

void show_service() // hien thi ngguoi dung len man hinh
{
    system("clear");
    printf("%-5s%-20s%-20s\n", "STT", "File descriptor", "Chat name");
    for (int i = 0; i < register_size; i++)
    {
        printf("%-5d%-20d%-20s\n", i, register_list[i].fd, register_list[i].chat_name);
    }
}

void broadcast(int user_fd, char *message)  // broadcast tin nhan cho tat ca nguoi dung
{
    char buffer[1000], name[100];
    uint16_t sent_byte;

    // kiem tra nguoi gui la ai, va khong gui lai cho nguoi day
    for (int i = 0; i < register_size; i++)
    {
        if (register_list[i].fd == user_fd)
        {
            strcpy(name, register_list[i].chat_name);
            break;
        }
    }

    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer, "%s: %s", name, message);  //  client1: message
    sent_byte = strlen(buffer);

    // gui tin nhan cho nhung nguoi con lai
    for (int i = 0; i < register_size; i++)
    {
        if (register_list[i].fd != user_fd)
        {
            write(register_list[i].fd, &sent_byte, sizeof(sent_byte));
            write(register_list[i].fd, buffer, sent_byte);
        }
    }
}

void *socket_handler(void *fun_arg) // accept client
{
    int *client_fd = (int *)fun_arg, login_result;
    uint16_t sent_byte, receive_byte;
    char message[1000], username[100];

    // printf("accept client: %d\n", *client_fd);

    memset(username, 0, sizeof(username));
    read(*client_fd, &receive_byte, sizeof(receive_byte));
    read(*client_fd, username, receive_byte);

    // printf("name: %s\n", username);
    add_service(*client_fd, username); // add nguoi dung vao he thong
    show_service(); // hien thi danh sach nguoi dung dang ket noi toi server

    while (1)  // lang nghe tin nhan cua nguoi dung
    {
        int read_status;
        memset(message, 0, sizeof(message));
        if (read(*client_fd, &receive_byte, sizeof(receive_byte)) == 0)
        {
            break;
        }
        if (read(*client_fd, message, receive_byte) == 0)
        {
            break;
        }

        // printf("%s: %s\n", username, message);
        broadcast(*client_fd, message); // neu co tin nhan, thi broadcast cho nhung nguoi dung khacs
    }

out:
    remove_service(*client_fd);
    show_service();
    close(*client_fd);
    free(client_fd);
}

int main()
{
    int server_fd, len, otp;
    struct sockaddr_in server_addr, client_addr;

    // khoi tao server
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        fprintf(stderr, "create socket error\n");
        exit(0);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;  // set ipv4
    server_addr.sin_port = htons(PORT_NUM); // set port
    server_addr.sin_addr.s_addr = INADDR_ANY; // tu dong lay dia chi ip de gan cho server
    otp = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &otp, sizeof(otp)); // config socket

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)  // bind cau truc socket cho file descripter
    {
        fprintf(stderr, "binding error\n");
        exit(0);
    }

    if (listen(server_fd, LISTEN_BACKLOG) == -1) // set listen backlog
    {
        fprintf(stderr, "listen error\n");
        exit(0);
    }

    printf("listening at port %d\n", PORT_NUM);

    while (1) // lang nghe ket noi tu nguoi dung
    {
        int *pclient_fd = malloc(sizeof(int));
        pthread_t thread;
        *pclient_fd = accept(server_fd, (struct sockaddr *)&client_addr, &len);  // neu co nguoi dung ket noi thi accept
        if (*pclient_fd == -1)
        {
            fprintf(stderr, "accept error\n");
            exit(0);
        }
        int a =pthread_create(&thread, NULL, socket_handler, pclient_fd); // tao thead xu ly tin nhan nguoi dung
    }

    return 0;
}
