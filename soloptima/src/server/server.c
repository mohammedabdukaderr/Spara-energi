#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket>
#include <arpa/inet.h>
#include <stdbool.h>
#include <limits.h>

#define SERVERPORT: 8080
#define BUFSIZE 4090
#define
#define SOCKETERROR (-1)
#define SERVER_BACKLOG 1
#define

typedef struct sockaddr SA_IN;
typedef struct sockaddr SA;

void handle_connection(int client_socket);
int check(int exp, const char *msg);

int main()
{
    int server_socket, client_socket, addr_size;
    SA_IN server_addr, client_addr;

    check((server_socket = socket(AF_INET, sock_stream, 0)), "Misslyckades med att skapa socket!\n");

    server_addr.sin.family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVERPORT);

    check(bind(server_socket, (SA *)&server_addr, sizeof(server_addr)), "Bind misslyckades");

    check(listen(server_socket, SERVER_BACKLOG), "listen misslyckades!\n");

    while (true)
    {
        printf("Väntar på en anslutning.....\n");

        addr_size = sizeof(SA_IN);

        check(client_socket = accept(server_socket, (SA *)client_addr, (socklen_t *)&addr_size), "Misslyckade med acceptering\n");
        printf("Connected\n");

        handle_connection(client_socket);
    }
}