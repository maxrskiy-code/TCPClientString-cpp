#include <iostream>
#include <string>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

namespace msocket {
	class Client {
		std::string server_ip;
		unsigned int server_port;
		void (*handlePacket)(msocket::Client* client, std::string connection);
		static void thread_recv(msocket::Client* client) {
			while (true) {
				std::string data = client->recv_string();
				client->handlePacket(client, data);
			}
		}
	public:
		bool status = false;
		SOCKET sock;
		Client(std::string server_ip, unsigned int server_port, void (*handlePacket)(msocket::Client* client, std::string connection)) {
			this->server_ip = server_ip;
			this->server_port = server_port;
			this->handlePacket = handlePacket;
		}

		bool start() {
			WSAData data;
			WORD ver = MAKEWORD(2, 2);
			int wsResult = WSAStartup(ver, &data);
			if (wsResult != 0)
			{
				return 0;
			}

			// Create socket
			sock = socket(AF_INET, SOCK_STREAM, 0);
			if (sock == INVALID_SOCKET)
			{
				WSACleanup();
				return 0;
			}

			// Fill in a hint structure
			sockaddr_in hint;
			hint.sin_family = AF_INET;
			hint.sin_port = htons(this->server_port);
			inet_pton(AF_INET, this->server_ip.c_str(), &hint.sin_addr);

			// Connect to server
			int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
			if (connResult == SOCKET_ERROR)
			{
				closesocket(sock);
				WSACleanup();
				return 0;
			}
			this->status = true;
			CreateThread(0, 0, (LPTHREAD_START_ROUTINE)thread_recv, (LPVOID)this, 0, 0);
			return 1;
		}

		bool send_string(std::string data) {
			int sendResult = send(this->sock, data.c_str(), data.size() + 1, 0);
			if (sendResult == SOCKET_ERROR) return 0;
			return 1;
		}

		std::string recv_string() {
			char buf[4096];
			ZeroMemory(buf, 4096);
			int bytesReceived = recv(sock, buf, 4096, 0);
			if (bytesReceived > 0)
			{
				// Echo response to console
				return std::string(buf, 0, bytesReceived);
			}
			return NULL;
		}
	};
}
