#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

struct Server
{
    int domain;
    int service;
    int protocol;
    u_long inter_face;
    int port;
    int backlong;

    struct sockaddr_in address;

    int socket;

    void (*launch)(struct Server *server);
};

struct Server server_constractor(int domain, int service, int protocol,
u_long inter_face, int port, int backlong, void (*launch)(struct Server *server));


struct Server server_constractor(int domain, int service, int protocol, u_long inter_face, 
int port, int backlong, void (*launch)(struct Server *server))
{
    struct Server server;

    server.domain = domain;
    server.service = service;
    server.protocol = protocol;
    server.inter_face = inter_face;
    server.port = port;
    server.backlong = backlong;

    server.address.sin_family = domain;
    server.address.sin_port = htons(port);
    server.address.sin_addr.s_addr = htonl(inter_face);

    server.socket = socket(domain, service, protocol);
    if(server.socket == 0)
    { 
        perror("Failed to connect socket...\n");
        exit(1);
    }

    if(bind(server.socket, (struct sockaddr *) &server.address, sizeof(server.address)) < 0)
    {
        perror("Failed to bind socket...\n");
        exit(1);
    }

    if(listen(server.socket, server.backlong) < 0)
    {
        perror("Failed to start listening...\n");
        exit(1);
    }

    server.launch = launch;

    return server;
        
}

void launch(struct Server *server)
{

    char buffer[30000];
    char *hello = "HTTP/1.1 200 OK\nDate: Mon, 27 Jul 2009 12:28:53 GMT\nServer: Apache/2.2.14 (Win32)\nLast-Modified: Wed, 22 Jul 2009 19:15:56 GMT\nContent-Type: text/html\nConnection: Closed\n\n<html><body><h1>Hello,World!</h1></body></html>";
    int address_lenght = sizeof(server->address);
    int new_socket;
    while(1)
    {
        printf("======= WAITING FOR CONNECTION =======\n");
        new_socket = accept(server->socket, (struct sockaddr *) &server->address, (socklen_t *) &address_lenght);
        read(new_socket, buffer, 30000);
        printf("%s\n", buffer);
        write(new_socket, hello, strlen(hello));
        close(new_socket);
    }
}

int main()
{
    struct Server server = server_constractor(AF_INET, SOCK_STREAM, 0, INADDR_ANY, 80, 10, launch);
    server.launch(&server);
    return 0;
}
