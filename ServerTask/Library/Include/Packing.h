#ifndef TYPE_H
#define TYPE_H

#include <iostream>
#include <WinSock2.h>
#include <vector>
#include <unordered_map>

#pragma comment(lib, "ws2_32.lib")

#define SERVERIP	"127.0.0.1"
#define SERVERPORT	9001

using namespace std;

const int bufSize = 1028;

// Unordered_map�� �̿��ؼ� ���� ����
enum class PROTOCOL
{
	NONE = -1,
	C_CONNECT = 100,			// Ŭ�� �����ϸ� ù��°�� �����ش�.
	C_START,					// ������ �����մϴ�.
	C_EXIT,						// �������ϴ�.

	C_ROOM,
	C_ROOM_CREATE,

	C_READ,

	C_GAME_WAIT,

	C_DIR,
	C_BLOCK_DOWN,

	// ----------------------------------------------

	S_START = 200,					// 
	S_ROOM,
	S_ROOM_UPDATE,
	S_GAME_WAIT,
	S_GAME_START,					// ������ �����մϴ�.
	S_GAME_BOARD_INFO,				// ������ ������ ���� �����մϴ�.
	S_GAME_BOARD_LINE_CLEAR,		// ���� ������ Ŭ����Ǵ� ������ �����մϴ�.
	S_GAME_FAILED,					// ���ӿ��� �����ϴ�.
	S_GAME_WIN,						// ���ӿ��� �̰���ϴ�.




	EXIT = 300,						// ���Ż��
	END,
};

enum : int
{
	DOWN = 10,
	RIGHT,
	LEFT,
	SPACE,
	CHANGE
};

void ErrorCode(const char* str);

void PackCpy(char** ptr, void* val, int size);
void UnPackCpy(void* val, char** ptr, int size);
int Packing(char** ptr);

// �������ݿ�
template<typename... Args>
int Packing(char** ptr, PROTOCOL data, Args... args);

template<typename... Args>
int Packing(char** ptr, int data, Args... args);

template<typename... Args>
int Packing(char** ptr, float data, Args... args);

template<typename... Args>
int Packing(char** ptr, const char* data, Args... args);

template<typename... Args>
int Packing(char** ptr, char* data, Args... args);


template<typename... Args>
int SendPacking(SOCKET socket, Args... info)
{
	char buf[bufSize] = { 0, };
	::memset(buf, 0, bufSize);

	char* ptr = buf;
	ptr += sizeof(int); 

	int packSize = Packing(&ptr, info...);

	ptr = buf;
	::memcpy(ptr, &packSize, sizeof(int));
	packSize += sizeof(int);


	return ::send(socket, buf, packSize, 0);
}


void UnPacking(char** ptr);

template<typename... Args>
void UnPacking(char** ptr, PROTOCOL* data, Args... args);

template<typename... Args>
void UnPacking(char** ptr, int* data, Args... args);

template<typename... Args>
void UnPacking(char** ptr, float* data, Args... args);

template<typename... Args>
void UnPacking(char** ptr, char* data, Args... args);

template<typename ...Args>
inline void UnPacking(char** ptr, char& data, Args ...args);


// ------------------- ������ ---------------------------- //

template<typename ...Args>
inline int Packing(char** ptr, PROTOCOL data, Args ...args)
{
	PackCpy(ptr, &data, sizeof(PROTOCOL));
	return Packing(ptr, args...) + sizeof(PROTOCOL);
}

template<typename ...Args>
inline int Packing(char** ptr, int data, Args ...args)
{
	PackCpy(ptr, &data, sizeof(int));
	return Packing(ptr, args...) + sizeof(int);
}

template<typename ...Args>
inline int Packing(char** ptr, float data, Args ...args)
{
	PackCpy(ptr, &data, sizeof(float));
	return Packing(ptr, args...) + sizeof(float);
}

template<typename ...Args>
inline int Packing(char** ptr, char data, Args ...args)
{
	PackCpy(ptr, &data, sizeof(char));
	return Packing(ptr, args...) + sizeof(char);
}

template<typename ...Args>
inline int Packing(char** ptr, const char* data, Args ...args)
{
	return Packing(ptr, (char*)data, args...);
}

template<typename ...Args>
inline int Packing(char** ptr, char* data, Args ...args)
{
	char* temp = (char*)(data);
	int strSize = ::strlen(temp);
	PackCpy(ptr, &strSize, sizeof(int));
	PackCpy(ptr, temp, strSize);

	return Packing(ptr, args...) + strSize + sizeof(int);
}



template<typename ...Args>
inline void UnPacking(char** ptr, PROTOCOL* data, Args ...args)
{
	UnPackCpy(data, ptr, sizeof(PROTOCOL));
	UnPacking(ptr, args...);
	return;
}

template<typename ...Args>
inline void UnPacking(char** ptr, int* data, Args ...args)
{
	UnPackCpy(data, ptr, sizeof(int));
	UnPacking(ptr, args...);
	return;
}

template<typename ...Args>
inline void UnPacking(char** ptr, float* data, Args ...args)
{
	UnPackCpy(data, ptr, sizeof(float));
	UnPacking(ptr, args...);
	return;
}

template<typename ...Args>
inline void UnPacking(char** ptr, char& data, Args ...args)
{
	UnPackCpy(&data, ptr, sizeof(char));
	UnPacking(ptr, args...);
	return;
}

template<typename ...Args>
inline void UnPacking(char** ptr, char* data, Args ...args)
{
	int strSize = 0;
	UnPackCpy(&strSize, ptr, sizeof(int));
	UnPackCpy(data, ptr, strSize);
	UnPacking(ptr, args...);
	return;
}

inline int RecvPacking(SOCKET socket, char* buffer)
{
	char* ptr = buffer;
	int packSize = 0;
	::memset(buffer, 0, bufSize);

	// ��Ŷ�� ũ�⸦ �޴´�.
	int retValue = ::recv(socket, (char*)&packSize, sizeof(int), 0);
	if (retValue == SOCKET_ERROR) {
		return SOCKET_ERROR;
	}
	else if (retValue == 0) {
		return 0;
	}

	// ��Ŷ�� �޴´�.
	retValue = ::recv(socket, buffer, packSize, 0);
	if (retValue == SOCKET_ERROR) {
		return SOCKET_ERROR;
	}
	else if (retValue == 0) {
		return 0;
	}

	return 1;
}



#endif

