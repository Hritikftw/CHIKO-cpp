#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

class FileManager {
public:
    static const std::string BASE_DIR;

    // ---------- USER SETUP ----------
    static void createUser(const std::string& username) {
        std::string userDir = BASE_DIR + "/" + username;
        fs::create_directories(userDir);
        fs::create_directories(userDir + "/chats");

        std::string friendsPath = userDir + "/friends.txt";
        if (!fs::exists(friendsPath)) {
            std::ofstream f(friendsPath);
        }

        std::string requestsPath = userDir + "/requests.txt";
        if (!fs::exists(requestsPath)) {
            std::ofstream f(requestsPath);
        }
    }

    // ---------- FRIENDS ----------
    static std::vector<std::string> getFriends(const std::string& user) {
        return readLines(BASE_DIR + "/" + user + "/friends.txt");
    }

    static std::string formatFriendsWithIndex(const std::string& user,
                                               const std::vector<std::string>& friends) {
        std::string result;
        int i = 1;
        for (const auto& f : friends) {
            result += std::to_string(i++) + ". " + f + "\n";
        }
        return result;
    }

    static std::string getFriendByIndex(const std::string& user, int index) {
        auto friends = getFriends(user);
        if (index < 1 || index > (int)friends.size()) return "";
        return friends[index - 1];
    }

    // ---------- MESSAGES ----------
    static void sendMessage(const std::string& sender, const std::string& receiver,
                            const std::string& message) {
        if (!isFriend(sender, receiver)) return;

        createUser(sender);
        createUser(receiver);

        std::string senderChat  = BASE_DIR + "/" + sender  + "/chats/" + receiver + ".txt";
        std::string receiverChat = BASE_DIR + "/" + receiver + "/chats/" + sender  + ".txt";

        writeLine(senderChat,  "[you] " + message);
        writeLine(receiverChat, "[" + sender + "] " + message);
    }

    static std::string readChat(const std::string& user, const std::string& friendName) {
        std::string path = BASE_DIR + "/" + user + "/chats/" + friendName + ".txt";
        if (!fs::exists(path)) return "No messages yet.";
        std::string content = readFile(path);
        return content.empty() ? "No messages yet." : content;
    }

    // ---------- FRIEND REQUESTS ----------
    static void sendFriendRequest(const std::string& from, const std::string& to) {
        if (from == to) return;

        createUser(from);
        createUser(to);

        std::string reqFile = BASE_DIR + "/" + to + "/requests.txt";

        if (!fileContains(reqFile, from)) {
            writeLine(reqFile, from);
        }
    }

    static std::string listRequests(const std::string& user) {
        std::string path = BASE_DIR + "/" + user + "/requests.txt";
        std::string content = readFile(path);
        return content.empty() ? "No pending requests." : content;
    }

    static std::string getRequestByIndex(const std::string& user, int index) {
        auto reqs = readLines(BASE_DIR + "/" + user + "/requests.txt");
        if (index < 1 || index > (int)reqs.size()) return "";
        return reqs[index - 1];
    }

    static void acceptRequest(const std::string& user, const std::string& friendName) {
        addFriend(user, friendName);
        addFriend(friendName, user);
        removeLine(BASE_DIR + "/" + user + "/requests.txt", friendName);
    }

    static void rejectRequest(const std::string& user, const std::string& friendName) {
        removeLine(BASE_DIR + "/" + user + "/requests.txt", friendName);
    }

private:
    static bool isFriend(const std::string& u1, const std::string& u2) {
        return fileContains(BASE_DIR + "/" + u1 + "/friends.txt", u2);
    }

    static void addFriend(const std::string& user, const std::string& friendName) {
        std::string path = BASE_DIR + "/" + user + "/friends.txt";
        if (!fileContains(path, friendName)) {
            writeLine(path, friendName);
        }
    }

    static void writeLine(const std::string& path, const std::string& line) {
        std::ofstream f(path, std::ios::app);
        f << line << "\n";
    }

    static std::string readFile(const std::string& path) {
        if (!fs::exists(path)) return "";
        std::ifstream f(path);
        std::ostringstream ss;
        ss << f.rdbuf();
        return ss.str();
    }

    static std::vector<std::string> readLines(const std::string& path) {
        std::vector<std::string> lines;
        if (!fs::exists(path)) return lines;
        std::ifstream f(path);
        std::string line;
        while (std::getline(f, line)) {
            if (!line.empty()) lines.push_back(line);
        }
        return lines;
    }

    static bool fileContains(const std::string& path, const std::string& value) {
        auto lines = readLines(path);
        return std::find(lines.begin(), lines.end(), value) != lines.end();
    }

    static void removeLine(const std::string& path, const std::string& value) {
        auto lines = readLines(path);
        std::ofstream f(path, std::ios::trunc);
        for (const auto& l : lines) {
            if (l != value) f << l << "\n";
        }
    }
};

// Definition of the static member
const std::string FileManager::BASE_DIR = "users";
