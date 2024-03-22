#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
// #include "Server.h"

//struct
struct Server
{
    int domain;
    int service;
    int protocol;
    __u_long inter_face;
    int port;
    int backlong;

    struct sockaddr_in address;

    int socket;

    void (*launch)(struct Server *server);
};

struct HTTP_request
{
    char* method;
    char* action;
};

//inteface/dichiaration

struct Server server_constractor(int domain, int service, int protocol,
__u_long inter_face, int port, int backlong, void (*launch)(struct Server *server));

struct HTTP_request get_requets(char* http);

char* read_file(const char* filename);

//implementations/function


struct Server server_constractor(int domain, int service, int protocol, __u_long inter_face, 
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

struct HTTP_request get_requets(char* http)
{
    
    struct HTTP_request http_request;
    char* parser = http;

    int i = 0;

    while(*(parser+i) != ' ')
    {
        i++;
    }
    http_request.method = (char*)malloc(sizeof(i+1)*sizeof(char));
    
    i = 0;
    while(*(parser+i) != ' ')
    {
        http_request.method[i] = *(parser+i);
        i++;
    }
    http_request.method[i] = '\0';
    i+=1;

    int start_action = i;
    int j = 0;
    while(*(parser+i) != ' ')
    {
        i++;
        j++;
    }
    http_request.action = (char*)malloc(sizeof(j+1)*sizeof(char));
    
    i = start_action;
    j = 0;
    while(*(parser+i) != ' ')
    {
        http_request.action[j] = *(parser+i);
        i++;
        j++;
    }
    http_request.action[j] = '\0';
    
    return http_request;
}

char* read_file(const char* filename) {
    FILE *file;
    char *buffer;
    long fileLength;

    // Open the file for reading
    file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file\n");
        return NULL;
    }

    // Get the length of the file
    fseek(file, 0, SEEK_END);
    fileLength = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate memory for the buffer
    buffer = (char *)malloc(fileLength + 1);
    if (buffer == NULL) {
        printf("Memory allocation failed\n");
        return NULL;
    }

    // Read the file into the buffer
    fread(buffer, fileLength, 1, file);
    buffer[fileLength] = '\0'; // Add null terminator to the end of the string

    // Close the file
    fclose(file);
    return buffer;
}

void send_file_html(int socket, char* html)
{
    const char pre_html[] = "HTTP/1.1 200 OK\n\n";
    // printf("%s", html);
    char* complete_html = (char*) malloc((strlen(pre_html)+strlen(html)+1)*sizeof(char));
    complete_html[0] = '\0';
    strcat(complete_html, pre_html);
    strcat(complete_html, html);
    // printf("%s", complete_html);
    write(socket, complete_html, strlen(complete_html));
    free(complete_html);
    free(html);
}

void launch(struct Server *server)
{

    char buffer[30000];
    // char *hello = "HTTP/1.1 200 OK\nDate: Mon, 27 Jul 2009 12:28:53 GMT\nServer: Apache/2.2.14 (Win32)\nLast-Modified: Wed, 22 Jul 2009 19:15:56 GMT\nContent-Type: text/html\nConnection: Closed\n\n<html><body><h1>Hello,World!</h1></body></html>";
    // char *hello = "HTTP/1.1 200 OK\nConnection: Closed\n\n<html><body><h1>Hello,World!</h1></body></html>";
    // char *hello = "HTTP/1.1 200 OK\n\n<html><body><h1>Hello,World!</h1></body></html>";
    // const char pre_html[] = "HTTP/1.1 200 OK\n\n";
    int address_lenght = sizeof(server->address);
    int new_socket;
    int i = 0;
    while(1)
    {
        // char* buffer = (char*)malloc(30000*sizeof(char));
        if(buffer == NULL)
        {
            perror("Allocation buffer fail...\n");
            exit(1);
        }
        printf("%d======= WAITING FOR CONNECTION =======\n", i);
        new_socket = accept(server->socket, (struct sockaddr *) &server->address, (socklen_t *) &address_lenght);
        read(new_socket, buffer, 30000);
        printf("%s\n", buffer);
        struct HTTP_request http_request = get_requets(buffer);
        if((strcmp(http_request.method, "GET") == 0) && (strcmp(http_request.action, "/") == 0))
        {
            char* html = read_file("test.html");
            if(html != NULL)
            {
                send_file_html(new_socket, html);
            }
        }
        if((strcmp(http_request.method, "GET") == 0) && (strcmp(http_request.action, "/ok") == 0))
        {
            char* html = read_file("ok.html");
            if(html != NULL)
            {
                send_file_html(new_socket, html);
            }
        }
        close(new_socket);
        // free(buffer);
        i+=1;
    }
}

int main()
{
    struct Server server = server_constractor(AF_INET, SOCK_STREAM, 0, INADDR_ANY, 8080, 10, launch);
    server.launch(&server);
    return 0;
}
