#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

// --- CTML Configuration ---
#define CTML_PRETTY 
#define CTML_CUSTOM_ATTRIBUTES X(onclick);
#define CTML_IMPLEMENTATION
#include "ctml.h"
#include "ctml_short.h"

// --- The Sink Function ---
// Receives the HTML chunk and the client socket FD via userData
void socket_sink(char* data, void* userData) {
    int client_fd = *(int*)userData;
    send(client_fd, data, strlen(data), MSG_NOSIGNAL);
}

// --- HTML Page Generator ---
void render_homepage(int client_fd, int visitor_count) {
    // Pass the client_fd address as userData to the context
    ctml(.sink = socket_sink, .userData = &client_fd) {
        
       ctml_raw("<!DOCTYPE html>");
        html(.lang="en") {
            head() {
                title() {ctml_raw("CTML Server"); }
                h(style) {
                   ctml_raw("body { font-family: sans-serif; text-align: center; padding: 50px; }");
                   ctml_raw(".box { border: 2px solid #333; padding: 20px; display: inline-block; }");
                }
            }
            body() {
                div(.class="box") {
                    h1() {ctml_raw("Hello from C!"); }
                    p() {ctml_raw("This HTML was generated directly by the CTML library."); }
                    
                    hr();
                    
                    div() {
                        p() {ctml_rawf("You are visitor number: <strong>%d</strong>", visitor_count);}
                        p() {ctml_text("You are visitor number: '' \" <> &  <strong>X</strong>");}
                    }
                    
                    br();
                    
                    button(.onclick="location.reload()") {
	                ctml_raw("Refresh Page");
                    }
                }
            }
        }
    }
}

// --- Main Server Loop ---
int main() {
    int server_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int visitor_count = 0;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on http://localhost:8080\n");

    while (1) {
        int client_fd;
        if ((client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept failed");
            continue;
        }

        visitor_count++;

        // Read request (ignored)
        char buffer[1024] = {0};
        read(client_fd, buffer, 1024);

        // Send Header
        char *header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n";
        send(client_fd, header, strlen(header), 0);

        // Render Body using CTML with local client_fd
        render_homepage(client_fd, visitor_count);

        close(client_fd);
    }

    return 0;
}
