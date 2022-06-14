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

// Unordered_map을 이용해서 만들 예정
enum class PROTOCOL
{
	NONE = -1,
	C_CONNECT = 100,			// 클라가 접속하면 첫번째로 보내준다.
	C_START,					// 게임을 시작합니다.
	C_EXIT,						// 나갔습니다.

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
	S_GAME_START,					// 게임을 시작합니다.
	S_GAME_BOARD_INFO,				// 보드의 정보를 전부 전달합니다.
	S_GAME_BOARD_LINE_CLEAR,		// 보드 라인이 클리어되는 정보를 전달합니다.
	S_GAME_FAILED,					// 게임에서 졌습니다.
	S_GAME_WIN,						// 게임에서 이겼습니다.




	EXIT = 300,						// 긴급탈출
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

// 프로토콜용
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


// ------------------- 구현부 ---------------------------- //

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

	// 패킷의 크기를 받는다.
	int retValue = ::recv(socket, (char*)&packSize, sizeof(int), 0);
	if (retValue == SOCKET_ERROR) {
		return SOCKET_ERROR;
	}
	else if (retValue == 0) {
		return 0;
	}

	// 패킷을 받는다.
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

