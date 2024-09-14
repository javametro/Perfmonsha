#include <iostream>
#include <cstdlib>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libssl.lib")
#pragma comment(lib, "libcrypto.lib")

void handle_request(SSL* ssl) {
    char buffer[1024];
    int bytes = SSL_read(ssl, buffer, sizeof(buffer) - 1);
    buffer[bytes] = '\0';
    std::cout << "Request: " << buffer << std::endl;

    const char* response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 13\r\n"
        "\r\n"
        "Hello World!\n";

    SSL_write(ssl, response, strlen(response));
}


int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    const SSL_METHOD* method = SSLv23_server_method();
    SSL_CTX* ctx = SSL_CTX_new(method);

    if (SSL_CTX_use_certificate_file(ctx, "server.crt", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, "server.key", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    SOCKET server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == INVALID_SOCKET) {
        perror("Unable to create socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(12345); // Port 12345
    sa.sin_addr.s_addr = INADDR_ANY;

    if (bind(server, (struct sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR) {
        perror("Unable to bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server, 10) == SOCKET_ERROR) {
        perror("Unable to listen");
        exit(EXIT_FAILURE);
    }

    while (1) {
        struct sockaddr_in addr;
        int len = sizeof(addr);
        SSL* ssl;
        SOCKET client = accept(server, (struct sockaddr*)&addr, &len);
        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, client);
        SSL_accept(ssl);
        // Handle the request here

        handle_request(ssl);

        SSL_shutdown(ssl);
        SSL_free(ssl);
        closesocket(client);
    }

    WSACleanup();
    return 0;
}
