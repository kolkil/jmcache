#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#define PORT 2137

int get_menu()
{
    printf("[1] - insert \"key\" \"value\"\n");
    printf("[2] - get \"key\"\n");
    int res = 0;
    if (scanf("%d", &res) != 1)
        return 0;
    return res;
}

int insert(char buffer[2048])
{
    memset(buffer, 0, 2048);
    if (scanf("%s", buffer) != 1)
        return 0;
    if (scanf("%s", buffer + 2048 / 3) != 1)
        return 0;
    if (scanf("%s", buffer + (2 * 2048) / 3))
        return 0;
    return 1;
}

int get(char buffer[2048])
{
    memset(buffer, 0, 2048);
    if (scanf("%s", buffer) != 1)
        return 0;
    if (scanf("%s", buffer + 2048 / 2) != 1)
        return 0;
    return 1;
}

int main()
{
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    char buffer[1024] = {0};
    while (1)
    {
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            printf("\n Socket creation error \n");
            return -1;
        }

        memset(&serv_addr, '0', sizeof(serv_addr));

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);

        if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
        {
            printf("\nInvalid address/ Address not supported \n");
            return -1;
        }

        char command_buffer[2048] = {0};

        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        {
            printf("\nConnection Failed \n");
            return -1;
        }
        int x = get_menu();
        switch (x)
        {
        case 1:
            insert(command_buffer);
            break;
        case 2:
            get(command_buffer);
            break;
        default:
            break;
        }
        for (int i = 0; i < 2046; ++i)
            if (command_buffer[i] == 0)
                command_buffer[i] = ' ';
        printf("%s\n", command_buffer);
        if (x = send(sock, command_buffer, 2048, 0), x < 1)
        {
            printf("send error %d\n", x);
            return -1;
        }
        valread = read(sock, buffer, 1024);
        printf("%s\n", buffer);
    }
    return 0;
}
