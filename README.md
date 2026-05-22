# CHIKO — C++ Edition
**Comprehensive Hub for Instant kommunication Operations**
A lightweight, C++-based LAN chat application for pure terminal communication. No internet. No cloud. Just machines talking to machines.

---

## ✨ Features

- 🔗 Multi-client communication over LAN
- 👥 Friend-only chats for controlled messaging
- 💬 Conversation-based chat system
- 📁 Offline message storage using file handling
- 🖥️ Clean and minimal terminal UI
- ⚡ Real-time messaging via C++ TCP sockets
- 🌐 No internet required — works entirely on local networks

---

## 🛠️ Technologies Used

- C++17
- Socket Programming (POSIX on Linux/macOS, Winsock2 on Windows)
- `std::thread` for multi-client handling
- `std::filesystem` for file-based storage
- No third-party libraries — pure standard C++

---

## 🚀 How It Works

1. A server runs on one machine within the LAN.
2. Multiple clients connect to the server using its IP address and port.
3. Messages are exchanged over raw TCP sockets using a lightweight `BEGIN…END` block protocol.
4. Conversations and offline messages are stored as plain text files.
5. When a user reconnects, missed messages are retrieved automatically.

---

## 🗂️ Project Structure

```
CHIKO-cpp/
├── server/
│   ├── ChatServer.cpp      ← Server entry point — binds port 5000, accepts clients
│   ├── ClientHandler.h     ← Per-client thread — handles all protocol commands
│   └── FileManager.h       ← File-based storage — friends, chats, requests
├── client/
│   └── ChatClient.cpp      ← Terminal client
├── Makefile                ← Linux/macOS build
├── CMakeLists.txt          ← Cross-platform CMake build
└── README.md
```

---

## ▶️ How to Run the Application

### ✅ Prerequisites

- GCC 8+ / Clang 8+ / MSVC 2019+ with C++17 support
- All devices must be on the same LAN / Wi-Fi network

---

### 🖥️ Step 1: Build the Project

**Linux / macOS**
```bash
make
```

**Windows (CMake)**
```bash
mkdir build && cd build
cmake ..
cmake --build .
```

**Windows (MSVC manual)**
```bat
cl /EHsc /std:c++17 /Fe:ChatServer.exe server\ChatServer.cpp ws2_32.lib
cl /EHsc /std:c++17 /Fe:ChatClient.exe client\ChatClient.cpp ws2_32.lib
```

---

### 🖥️ Step 2: Start the Server

Open a terminal on the server machine and run:
```bash
./ChatServer
# CHIKO Chat Server started on port 5000
```

Keep this terminal running — the server must stay active for clients to connect.

---

### 🌐 Step 3: Find Server IP Address

On the server machine:

**Windows**
```
ipconfig
```

**Linux / macOS**
```
ifconfig
```

Note the IPv4 address (example: `192.168.1.25`).

---

### 💻 Step 4: Run the Client (On Any Device)

Open a terminal on the client machine and run:
```bash
./ChatClient
```

---

### 🔐 Step 5: Login

When prompted:
```
Enter server IP: 192.168.1.25
Enter server port: 5000
Enter username: alice
```

If the username is new, it will be created automatically.

---

### 📋 Step 6: Chat Dashboard

After login, you will see:
```
---------------------------------------------------------------
                          CHIKO
---------------------------------------------------------------
1. INBOX   2. PENDING REQUESTS   3. SEND FRIEND REQUEST   4. LOGOUT
```

---

### 💬 Step 7: Chatting

- Enter a friend's number to open a chat
- Type messages and press **Enter** to send them instantly
- Press **Enter** on an empty line to refresh and see new messages
- Enter `0` to go back to the dashboard
- Conversations display clearly as:
  ```
  [friend] hello there
  [you] hey!
  ```

---

### 📴 Offline Messaging Support

- Sender must be online
- Receiver can be offline
- Messages are stored on the server and delivered when the receiver logs in

---

### 🛑 Step 8: Logout

From the dashboard, choose option `4`. The client disconnects safely from the server.

---

## ⚠️ Important Notes

- Internet connection is **NOT** required
- All devices must be on the **same network**
- Server must be running **before** clients connect
- Works on both same-device and multi-device setups

---

## 📁 Offline Message Storage

```
users/
└── alice/
    ├── friends.txt       ← one accepted friend per line
    ├── requests.txt      ← pending incoming friend requests
    └── chats/
        └── bob.txt       ← full conversation with bob
```

Messages sent to offline users are saved and delivered on reconnect. No database required — lightweight and fast.

---

## 🔒 Networking Scope

- Works only within the same LAN / Wi-Fi
- No external servers
- No internet dependency
- No third-party APIs

**CHIKO is ideal for:**
- College labs
- Hostels
- Offices
- Secure local environments

---

## 🧪 Sample Use Case

1. Start the server in a lab.
2. Students connect using their terminal.
3. Chat only with approved friends.
4. Disconnect and reconnect without losing messages.

---

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
