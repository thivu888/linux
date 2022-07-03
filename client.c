#include <fcntl.h>
#include <stdint.h>    
#include <pthread.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define PORT_NUM 4000
#define SERVER_ADDR "127.0.0.1"

int server_fd;
pthread_t receive_thread, sent_thread;


static void *receive_message(void *fun_arg)
{
    int *server_fd = (int *)fun_arg;
    uint16_t receive_byte;
    char buffer[1024], name[100], message[1024];
    int des_fd = open("/dev/driver_encode_device", O_RDWR);  // mo file driver aes
    while (1)
    {
        memset(buffer, 0, sizeof(buffer));
        memset(name, 0, sizeof(name));
        memset(message, 0, sizeof(message));

        // nhan tin nhat tu server, tin nhan da duoc ma hoa
        if (read(*server_fd, &receive_byte, sizeof(receive_byte)) == 0)
        {
            printf("disconnect from server\n");
            exit(0);
        }
        if (read(*server_fd, buffer, receive_byte) == 0)
        {
            printf("disconnect from server\n");
            exit(0);
        }
        int isHasName = 0;
        int n = 0, d = 0;
        for (size_t i = 0; i < strlen(buffer); i++)
        {
            if(isHasName == 0) {
                if(buffer[i] == ':') {
                    isHasName = 1;
                    continue;
                }
                name[n++] = buffer[i];
            }else {
                message[d++] = buffer[i];
            }
        }
        // giai ma tin nhan
        memset(buffer, 0, sizeof(buffer));
        buffer[0] = 2;
        for (size_t i = 0; i < strlen(message); i++)
        {
            buffer[i+1] = message[i];
        }
        write(des_fd, buffer, strlen(buffer));
        memset(message, 0, sizeof(message));
        read(des_fd, message, sizeof(message));
        printf("%s: %s\n", name, message); // in ra tin nhat da duoc giai ma
        memset(buffer, 0, sizeof(buffer));
        memset(name, 0, sizeof(name));
        memset(message, 0, sizeof(message));

    }
    close(des_fd);
}

static void *send_message(void *fun_arg)
{
    int *server_fd = (int *)fun_arg;
    uint16_t sent_byte;
    char message[1024], encrypt_message[1024];

    // mo file driver aes
    int des_fd = open("/dev/driver_encode_device", O_RDWR);
    if (des_fd == -1) {
        printf("khong mo dc file driver\n");
        // exit(0);
    }

    while (1)  // chat
    {
        memset(message, 0, sizeof(message));
        memset(encrypt_message, 0, sizeof(encrypt_message));
	    // nhap tin nhan tu ban phim
        fgets(message, sizeof(message), stdin);
        if (strlen(message) != 0) 
        {
            // ma hoa tin nhan bang file driver
            encrypt_message[0] = 1;
            for (size_t i = 0; i < strlen(message); i++)
            {
                encrypt_message[i+1] = message[i];
            }
            int lengEncrypt = strlen(encrypt_message);
            write(des_fd, encrypt_message, strlen(encrypt_message));

            memset(encrypt_message, 0, sizeof(encrypt_message));
            // doc ban tin da duoc ma hoa tu driver
            read(des_fd, encrypt_message, sizeof(encrypt_message));
            // gui tin nhan da duoc ma hoa cho server
            sent_byte = strlen(encrypt_message);
            // sent_byte = strlen(message);

            write(*server_fd, &sent_byte, sizeof(sent_byte));
            write(*server_fd, encrypt_message, sent_byte);
        }
        

    }
    close(des_fd);
}

int setname(int server_fd)  // ham setname 
{
    char name[100];
    uint16_t send_byte;

    // nhap ten tu ban phim, xong gui ten den server
    printf("chat name: ");
    fgets(name, sizeof(name), stdin);
    name[strlen(name) - 1] = 0;
    send_byte = strlen(name);  // so byte cua ten
    write(server_fd, &send_byte, sizeof(send_byte));  // gui do dai cua tin nhan
    write(server_fd, name, send_byte); // gui tin nhan
}

int main()
{
    int status, login_status;
    struct sockaddr_in server_addr;

    // khoi tao ket noi den server
    server_fd = socket(AF_INET, SOCK_STREAM, 0);  // tao file descripter cho socket, su dung de doc ghi
   

    if (server_fd == -1)  // neu khoi thao that bai thi thoat
    {
        fprintf(stderr, "create socket error\n");
        exit(0);
    }

    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;  // khoi tao ipv4
    server_addr.sin_port = htons(PORT_NUM);  // set port
    if (inet_pton(AF_INET, SERVER_ADDR, &server_addr.sin_addr) == -1) // set server address, neu thai bai thi thoat
    {
        fprintf(stderr, "server_addr fail\n");
        exit(0);
    }

    // connect toi server
    status = connect(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (status == -1)
    {
        fprintf(stderr, "connect error\n");
        exit(0);
    }

    printf("connect to server!\n");

    // set name cho client
    setname(server_fd);

    // tao thread nhan message
    pthread_create(&receive_thread, NULL, receive_message, &server_fd);
    // tao thread gui message
    pthread_create(&sent_thread, NULL, send_message, &server_fd);
    while (1)
        sleep(1);

    return 0;
}
