#pragma once
#include <string>
#include <thread>
#include <sstream>
#include <iostream>
#include "FileManager.h"

#ifdef _WIN32
  #include <winsock2.h>
  #include <ws2tcpip.h>
  using SOCK = SOCKET;
  #define INVALID_SOCK INVALID_SOCKET
  #define CLOSE_SOCK(s) closesocket(s)
#else
  #include <sys/socket.h>
  #include <unistd.h>
  using SOCK = int;
  #define INVALID_SOCK (-1)
  #define CLOSE_SOCK(s) close(s)
#endif

class ClientHandler {
public:
    explicit ClientHandler(SOCK clientSock)
        : sock(clientSock) {}

    // Launch the handler in a detached thread
    void start() {
        std::thread([this]() {
            run();
            delete this;   // self-cleanup after run() returns
        }).detach();
    }

private:
    SOCK        sock;
    std::string username;

    // ---- Low-level I/O ----

    // Send a full line (appends \n)
    void sendLine(const std::string& line) {
        std::string s = line + "\n";
        send(sock, s.c_str(), (int)s.size(), 0);
    }

    // Read a full line (strips \r\n)
    std::string recvLine() {
        std::string result;
        char ch;
        while (true) {
            int n = recv(sock, &ch, 1, 0);
            if (n <= 0) return "";
            if (ch == '\n') break;
            if (ch != '\r') result += ch;
        }
        return result;
    }

    // Send a BEGIN…END block (msg may contain \n)
    void sendBlock(const std::string& msg) {
        sendLine("BEGIN");
        std::istringstream ss(msg);
        std::string line;
        while (std::getline(ss, line)) {
            // strip stray \r
            if (!line.empty() && line.back() == '\r') line.pop_back();
            sendLine(line);
        }
        sendLine("END");
    }

    // ---- Main handler ----
    void run() {
        try {
            // LOGIN
            sendBlock("ENTER_USERNAME");
            username = recvLine();
            if (username.empty()) return;

            FileManager::createUser(username);
            sendBlock("LOGIN_SUCCESS");

            std::string command;
            while (!(command = recvLine()).empty()) {
                std::cout << "SERVER RECEIVED: [" << command << "]\n";

                if (command == "LIST_FRIENDS") {
                    auto friends = FileManager::getFriends(username);
                    if (friends.empty()) {
                        sendBlock("No friends added.\nAdd friends to chat.");
                    } else {
                        sendBlock(FileManager::formatFriendsWithIndex(username, friends));
                    }
                }

                else if (command.rfind("OPEN_CHAT ", 0) == 0) {
                    int index = std::stoi(command.substr(10));
                    std::string friendName = FileManager::getFriendByIndex(username, index);
                    if (friendName.empty()) {
                        sendBlock("Invalid selection.");
                    } else {
                        sendBlock(FileManager::readChat(username, friendName));
                    }
                }

                else if (command.rfind("SEND_REQUEST ", 0) == 0) {
                    std::string target = command.substr(13);
                    // trim leading/trailing spaces
                    target.erase(0, target.find_first_not_of(" \t"));
                    target.erase(target.find_last_not_of(" \t") + 1);
                    if (target.empty()) {
                        sendBlock("ERROR: Please provide a username.");
                    } else {
                        FileManager::sendFriendRequest(username, target);
                        sendBlock("Friend request sent to " + target);
                    }
                }

                else if (command.rfind("SEND ", 0) == 0) {
                    // Format: SEND <index> <message>
                    std::string rest = command.substr(5);
                    size_t sp = rest.find(' ');
                    if (sp == std::string::npos) {
                        sendBlock("Invalid SEND format.");
                        continue;
                    }
                    int index = std::stoi(rest.substr(0, sp));
                    std::string message = rest.substr(sp + 1);
                    std::string friendName = FileManager::getFriendByIndex(username, index);
                    if (friendName.empty()) {
                        sendBlock("Invalid friend selection.");
                    } else {
                        FileManager::sendMessage(username, friendName, message);
                        sendBlock("Message sent");
                    }
                }

                else if (command == "LIST_REQUESTS") {
                    sendBlock(FileManager::listRequests(username));
                }

                else if (command.rfind("ACCEPT_REQUEST ", 0) == 0) {
                    int index = std::stoi(command.substr(15));
                    std::string requester = FileManager::getRequestByIndex(username, index);
                    if (requester.empty()) {
                        sendBlock("Invalid request.");
                    } else {
                        FileManager::acceptRequest(username, requester);
                        sendBlock("Friend request accepted.");
                    }
                }

                else if (command.rfind("REJECT_REQUEST ", 0) == 0) {
                    int index = std::stoi(command.substr(15));
                    std::string requester = FileManager::getRequestByIndex(username, index);
                    if (requester.empty()) {
                        sendBlock("Invalid request.");
                    } else {
                        FileManager::rejectRequest(username, requester);
                        sendBlock("Friend request rejected.");
                    }
                }

                else if (command == "LOGOUT") {
                    sendBlock("LOGGED_OUT");
                    break;
                }

                else {
                    sendBlock("Unknown command");
                }
            }
        } catch (const std::exception& e) {
            std::cout << "Client disconnected: " << username
                      << " (" << e.what() << ")\n";
        }
        CLOSE_SOCK(sock);
    }
};
