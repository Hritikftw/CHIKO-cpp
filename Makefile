CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
LDFLAGS  = -lpthread

.PHONY: all server client clean

all: server client

server: server/ChatServer.cpp server/ClientHandler.h server/FileManager.h
	$(CXX) $(CXXFLAGS) -o ChatServer server/ChatServer.cpp $(LDFLAGS)

client: client/ChatClient.cpp
	$(CXX) $(CXXFLAGS) -o ChatClient client/ChatClient.cpp

clean:
	rm -f ChatServer ChatClient
