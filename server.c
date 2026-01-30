#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

// --- CTML Configuration ---
// Enable formatted text (FTEXT) which uses snprintf
// Enable pretty printing for readable HTML
#define CTML_PRETTY 
#define CTML_CUSTOM_ATTRIBUTES X(onclick);
#define CTML_IMPLEMENTATION
#include "ctml.h"
#include "ctml_short.h" // Allows using div{} instead of h(div){}

// --- Server Globals ---
// We need a global because the ctml sink callback cannot take user data.
int current_client_fd = -1;

// --- The Sink Function ---
// This function is called by CTML whenever it generates a piece of HTML.
void socket_sink(char* data) {
    if (current_client_fd >= 0) {
        // Send the data chunk directly to the client
        send(current_client_fd, data, strlen(data), 0);
    }
}

// --- HTML Page Generator ---
void render_homepage(int visitor_count) {
    // Initialize CTML with our socket_sink
    ctml(.sink = socket_sink) {
        
        t("<!DOCTYPE html>");
        html(.lang="en") {
            head() {
                title() { t("CTML Server"); }
                // Simple CSS embedded directly
                h(style) {
                    t("body { font-family: sans-serif; text-align: center; padding: 50px; }");
                    t(".box { border: 2px solid #333; padding: 20px; display: inline-block; }");
                }
            }
            body() {
                div(.class="box") {
                    h1() { t("Hello from C!"); }
                    p() { t("This HTML was generated directly by the CTML library."); }
                    
                    hr();
                    
                    // Example of dynamic content using FTEXT (formatted text)
                    p() {
                        FTEXT("You are visitor number: <strong>%d</strong>", visitor_count);
                    }
                    
                    br();
                    
                    button(.onclick="location.reload()") {
                        t("Refresh Page");
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

    // 1. Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 2. Bind to port 8080
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // 3. Listen for connections
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on http://localhost:8080\n");

    while (1) {
        // 4. Accept new connection
        if ((current_client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept failed");
            continue;
        }

        visitor_count++;

        // Read the HTTP request (we just ignore it for this simple example)
        char buffer[1024] = {0};
        read(current_client_fd, buffer, 1024);

        // 5. Send HTTP Header
        char *header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n";
        send(current_client_fd, header, strlen(header), 0);

        // 6. Generate and stream HTML Body using CTML
        render_homepage(visitor_count);

        // 7. Close connection
        close(current_client_fd);
        current_client_fd = -1;
    }

    return 0;
}
