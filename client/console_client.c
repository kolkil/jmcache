#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#define PORT 2137

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

        // Convert IPv4 and IPv6 addresses from text to binary form
        if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
        {
            printf("\nInvalid address/ Address not supported \n");
            return -1;
        }

        char command_buffer[1024] = {0},
             key_buffer[1024] = {0},
             value_buffer[1024] = {0},
             total_buffer[10024] = {0};

        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        {
            printf("\nConnection Failed \n");
            return -1;
        }
        int x = 0;
        if (scanf("%s", command_buffer) != 1)
            return 0;
        if (scanf("%s", key_buffer) != 1)
            return 0;
        if (scanf("%s", value_buffer) != 1)
            return 0;
        sprintf(total_buffer, "%s \"%s\" \"%s\"", command_buffer, key_buffer, value_buffer);
        printf("%s\n", total_buffer);
        if (x = send(sock, total_buffer, strlen(total_buffer), 0), x < 1)
        {
            printf("send error %d\n", x);
            return -1;
        }
        valread = read(sock, buffer, 1024);
        printf("%s\n", buffer);
    }
    return 0;
}
