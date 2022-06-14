#pragma once




/*---------------
	ClientInfo
----------------*/
struct ClientInfo
{
public:
	string name = "";

	uint32 socre = 0;
};

/*---------------
	SocketInfo
----------------*/
struct SocketInfo
{
public:
	SOCKET socket = NULL;
	SOCKADDR_IN addr = {};

	char sendBuffer[MAX_PATH] = { 0 };
	char recvBuffer[MAX_PATH] = { 0 };

	char* recvPtr = nullptr;
	char* sendPtr = nullptr;
};

