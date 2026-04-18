#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 8192
#define WWW_ROOT "www"

// Content-Type detection
const char* get_content_type(const char* path) {
    const char* ext = strrchr(path, '.');

    if (!ext) return "text/plain";

    if (strcmp(ext, ".html") == 0) return "text/html";
    if (strcmp(ext, ".css") == 0) return "text/css";
    if (strcmp(ext, ".js") == 0) return "application/javascript";
    if (strcmp(ext, ".png") == 0) return "image/png";
    if (strcmp(ext, ".jpg") == 0) return "image/jpeg";

    return "application/octet-stream";
}

// Send HTTP response
void send_response(SOCKET client, const char *status, const char *content_type, const char *body, int body_length) {
    char header[BUFFER_SIZE];

    int header_len = sprintf(header,
        "HTTP/1.1 %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n",
        status,
        content_type,
        body_length
    );

    send(client, header, header_len, 0);
    send(client, body, body_length, 0);
}

// Client handler (THREAD)
DWORD WINAPI handle_client_thread(LPVOID client_socket) {
    SOCKET client = (SOCKET)client_socket;

    char buffer[BUFFER_SIZE] = {0};
    recv(client, buffer, BUFFER_SIZE, 0);

    char method[10], path[256];
    sscanf(buffer, "%s %s", method, path);

    printf("---- New Request ----\n%s\n", buffer);
    printf("Requested Path: %s\n\n", path);

    if (strcmp(method, "GET") != 0) {
        char* msg = "<h1>405 Method Not Allowed</h1>";
        send_response(client, "405 Method Not Allowed", "text/html", msg, strlen(msg));
        closesocket(client);
        return 0;
    }

    // Normalize path
    if (strcmp(path, "/") == 0 || strlen(path) == 0) {
        strcpy(path, "/index.html");
    }

    char file_path[512];
    sprintf(file_path, "%s%s", WWW_ROOT, path);

    FILE *file = fopen(file_path, "rb");

    if (!file) {
        char* msg = "<h1>404 Not Found</h1>";
        send_response(client, "404 Not Found", "text/html", msg, strlen(msg));
        closesocket(client);
        return 0;
    }

    fseek(file, 0, SEEK_END);
    int file_size = ftell(file);
    rewind(file);

    char* body = malloc(file_size);
    fread(body, 1, file_size, file);
    fclose(file);

    const char* type = get_content_type(file_path);

    send_response(client, "200 OK", type, body, file_size);

    free(body);
    closesocket(client);

    return 0;
}

// Main server
int main() {
    WSADATA wsa;
    SOCKET server_fd, client_fd;
    struct sockaddr_in server, client;
    int c = sizeof(struct sockaddr_in);

    WSAStartup(MAKEWORD(2,2), &wsa);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&server, sizeof(server));
    listen(server_fd, 5);

    printf("🔥 Threaded Server running on port %d...\n", PORT);

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr *)&client, &c);

        // 🔥 Create new thread per client
        CreateThread(NULL, 0, handle_client_thread, (LPVOID)client_fd, 0, NULL);
    }

    closesocket(server_fd);
    WSACleanup();
    return 0;
}