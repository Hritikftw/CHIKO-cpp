#include <iostream>
#include <string>
#include <sstream>
#include <cstring>

#ifdef _WIN32
  #include <winsock2.h>
  #include <ws2tcpip.h>
  #pragma comment(lib, "ws2_32.lib")
  using SOCK = SOCKET;
  #define INVALID_SOCK INVALID_SOCKET
  #define CLOSE_SOCK(s) closesocket(s)
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <unistd.h>
  using SOCK = int;
  #define INVALID_SOCK (-1)
  #define CLOSE_SOCK(s) close(s)
#endif

// ---- Low-level socket helpers ----

static SOCK g_sock = INVALID_SOCK;

static void sendLine(const std::string& line) {
    std::string s = line + "\n";
    send(g_sock, s.c_str(), (int)s.size(), 0);
}

static std::string recvLine() {
    std::string result;
    char ch;
    while (true) {
        int n = recv(g_sock, &ch, 1, 0);
        if (n <= 0) return "";
        if (ch == '\n') break;
        if (ch != '\r') result += ch;
    }
    return result;
}

// Read and print a BEGIN…END block from the server
static void readBlock() {
    std::string line;
    while ((line = recvLine()) != "BEGIN") {
        if (line.empty()) return;
    }
    while ((line = recvLine()) != "END") {
        if (line.empty()) return;
        std::cout << line << "\n";
    }
}

// ---- Feature functions ----

static void openChat(const std::string& friendIndex) {
    sendLine("OPEN_CHAT " + friendIndex);
    readBlock();

    while (true) {
        std::cout << "> ";
        std::string msg;
        std::getline(std::cin, msg);

        if (msg == "0") return;

        if (msg.empty()) {
            // Refresh chat
            sendLine("OPEN_CHAT " + friendIndex);
            readBlock();
            continue;
        }

        sendLine("SEND " + friendIndex + " " + msg);
        readBlock();
    }
}

static void openInbox() {
    sendLine("LIST_FRIENDS");
    readBlock();

    std::cout << "Enter friend number to open chat\n";
    std::cout << "0. Back\n> ";

    std::string choice;
    std::getline(std::cin, choice);
    if (choice == "0") return;

    openChat(choice);
}

static void openPendingRequests() {
    sendLine("LIST_REQUESTS");
    readBlock();

    std::cout << "Enter number to ACCEPT\n";
    std::cout << "R<number> to REJECT\n";
    std::cout << "0. Back\n> ";

    std::string input;
    std::getline(std::cin, input);
    if (input == "0") return;

    if (!input.empty() && (input[0] == 'R' || input[0] == 'r')) {
        sendLine("REJECT_REQUEST " + input.substr(1));
    } else {
        sendLine("ACCEPT_REQUEST " + input);
    }

    readBlock();
}

static void sendFriendRequest() {
    std::cout << "Enter username: ";
    std::string user;
    std::getline(std::cin, user);

    sendLine("SEND_REQUEST " + user);
    readBlock();
}

// ---- Main ----

int main() {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed.\n";
        return 1;
    }
#endif

    std::cout << "Enter server IP: ";
    std::string serverIP;
    std::getline(std::cin, serverIP);

    std::cout << "Enter server port: ";
    std::string portStr;
    std::getline(std::cin, portStr);
    int port = std::stoi(portStr);

    g_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (g_sock == INVALID_SOCK) {
        std::cerr << "Socket creation failed.\n";
        return 1;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(port);
    inet_pton(AF_INET, serverIP.c_str(), &addr.sin_addr);

    if (connect(g_sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Connection failed. Is the server running?\n";
        CLOSE_SOCK(g_sock);
        return 1;
    }

    // ---- LOGIN ----
    readBlock();   // "ENTER_USERNAME" prompt

    std::cout << "Enter username: ";
    std::string username;
    std::getline(std::cin, username);
    sendLine(username);

    readBlock();   // "LOGIN_SUCCESS"

    // ---- Dashboard loop ----
    while (true) {
        std::cout << "\n---------------------------------------------------------------\n";
        std::cout << "                          CHIKO\n";
        std::cout << "---------------------------------------------------------------\n";
        std::cout << "1. INBOX   2. PENDING REQUESTS   3. SEND FRIEND REQUEST   4. LOGOUT\n";
        std::cout << "\nOPTION: ";

        std::string option;
        std::getline(std::cin, option);

        if (option == "1") {
            openInbox();
        } else if (option == "2") {
            openPendingRequests();
        } else if (option == "3") {
            sendFriendRequest();
        } else if (option == "4") {
            sendLine("LOGOUT");
            readBlock();
            break;
        } else {
            std::cout << "Invalid option.\n";
        }
    }

    CLOSE_SOCK(g_sock);

#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}
