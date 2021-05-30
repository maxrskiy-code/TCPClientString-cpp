#include <iostream>
#include <string>
#include <WS2tcpip.h>
#include "msocket.hpp"
#pragma comment(lib, "ws2_32.lib")

using namespace std;


void handlePacket(msocket::Client* client, std::string connection) {
	cout << "SERVER: " << connection << endl;
};


void main()
{
	msocket::Client sock_client = msocket::Client("127.0.0.1", 54000, handlePacket);
	bool success = sock_client.start();
	success = sock_client.send_string("Hello, server!");
	while (1);
}
