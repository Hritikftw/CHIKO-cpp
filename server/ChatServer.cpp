#include <iostream>
#include <cstring>
#include "ClientHandler.h"

#ifdef _WIN32
  #pragma comment(lib, "ws2_32.lib")
#else
  #include <netinet/in.h>
  #include <arpa/inet.h>
#endif

int main() {
    std::cout << "Starting CHIKO Chat Server...\n";

#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed.\n";
        return 1;
    }
#endif

    SOCK serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSock == INVALID_SOCK) {
        std::cerr << "Failed to create server socket.\n";
        return 1;
    }

    // Allow port reuse to avoid "Address already in use" on quick restarts
    int opt = 1;
#ifdef _WIN32
    setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
#else
    setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif

    sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(5000);

    if (bind(serverSock, (sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Bind failed.\n";
        CLOSE_SOCK(serverSock);
        return 1;
    }

    if (listen(serverSock, 10) < 0) {
        std::cerr << "Listen failed.\n";
        CLOSE_SOCK(serverSock);
        return 1;
    }

    std::cout << "CHIKO Chat Server started on port 5000\n";
    std::cout << "Waiting for clients...\n";

    while (true) {
        sockaddr_in clientAddr{};
        socklen_t clientLen = sizeof(clientAddr);
        SOCK clientSock = accept(serverSock, (sockaddr*)&clientAddr, &clientLen);
        if (clientSock == INVALID_SOCK) continue;

        // ClientHandler deletes itself when its thread finishes
        (new ClientHandler(clientSock))->start();
    }

    CLOSE_SOCK(serverSock);

#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}
