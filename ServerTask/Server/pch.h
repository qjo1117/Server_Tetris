#pragma once


#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <functional>

using namespace std;

#ifndef _PCH_
#define _PCH_

#define BUFSIZE 1028

// 비교 연산자 만들기 귀찮았습니다.
#define COMPARE(left, value, right) true == (##left value && value ##right)

// LockGuard를 사용하기 위한 초기셋팅
// 크리티컬섹션을 사용하는데 너무 긴 API를 써야되어 C++에서 지원하는 기능과
// 혼합하여 간단하게 사용할 수 있도록 해놓았습니다.
class Lock
{
public:
	Lock() { InitializeCriticalSection(&_lock); }
	~Lock() { DeleteCriticalSection(&_lock); }

	void lock() { EnterCriticalSection(&_lock); }
	void unlock() { LeaveCriticalSection(&_lock); }
private:
	CRITICAL_SECTION _lock = {};					// 크리티컬 섹션
};

#include "Packing.h"

enum class EventType
{
	NONE,
	Send,
	Recv,
	Accept,
};

class IocpEvent : public OVERLAPPED
{
public:
	EventType type = EventType::NONE;
	class SocketInfo* owner = nullptr;

	void Init(EventType p_type) { type = p_type; }
};

enum
{
	SOC_ERROR = 1,
	SOC_TRUE,
	SOC_FALSE
};

enum STATE
{
	MENU_SELECT_STATE = 1,
	MOVE_GAME_STATE,
	AUTO_MOVE_GAME_STATE,
	RESULT_STATE,
	RESULT_SEND_STATE,
	ROOM_STATE,
	DISCONNECTED_STATE
};


// 소켓 정보 저장을 위한 구조체
class SocketInfo
{
public:
	IocpEvent		r_overlapped;					// Recv_OVERLAPPED
	IocpEvent		s_overlapped;					// Send_OVERLAPPED
	SOCKET			sock;
	SOCKADDR_IN		addr;
	STATE			state = STATE::RESULT_STATE;	// 현재 상태
	bool			r_sizeflag = false;

	int				recvbytes = 0;
	int				comp_recvbytes = 0;
	int				sendbytes = 0;
	int				comp_sendbytes = 0;

	char			recvbuf[BUFSIZE] = { 0 };
	char			sendbuf[BUFSIZE] = { 0 };

	WSABUF			r_wsabuf = {};
	WSABUF			s_wsabuf = {};

	class Board*	board = nullptr;				// 보드정보랑 유저랑 1:1대응
	class Room*		room = nullptr;					// 룸과 유저는 N:1 대응
	string			name = "";						// 유저의 이름


	int				id = -1;						// 현재 유저의 아이디

	SocketInfo() {
		r_overlapped.Init(EventType::Recv);
		s_overlapped.Init(EventType::Send);

		r_overlapped.owner = this;
		s_overlapped.owner = this;
	}
	~SocketInfo() {
		if (board) {
			delete board;
		}
	}

	// 써야햇지만 급해서 안썼습니다.
	template<class... Args>
	int SendIOCPPack(Args... args)
	{
		char* sendPtr = sendbuf + sizeof(int);
		int size = Packing(&sendPtr, args...);
		memcpy(sendbuf, &size, sizeof(int));
		size += sizeof(int);

		return size;
	}
};

#endif