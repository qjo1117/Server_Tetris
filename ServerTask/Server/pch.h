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

// �� ������ ����� �����ҽ��ϴ�.
#define COMPARE(left, value, right) true == (##left value && value ##right)

// LockGuard�� ����ϱ� ���� �ʱ����
// ũ��Ƽ�ü����� ����ϴµ� �ʹ� �� API�� ��ߵǾ� C++���� �����ϴ� ��ɰ�
// ȥ���Ͽ� �����ϰ� ����� �� �ֵ��� �س��ҽ��ϴ�.
class Lock
{
public:
	Lock() { InitializeCriticalSection(&_lock); }
	~Lock() { DeleteCriticalSection(&_lock); }

	void lock() { EnterCriticalSection(&_lock); }
	void unlock() { LeaveCriticalSection(&_lock); }
private:
	CRITICAL_SECTION _lock = {};					// ũ��Ƽ�� ����
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


// ���� ���� ������ ���� ����ü
class SocketInfo
{
public:
	IocpEvent		r_overlapped;					// Recv_OVERLAPPED
	IocpEvent		s_overlapped;					// Send_OVERLAPPED
	SOCKET			sock;
	SOCKADDR_IN		addr;
	STATE			state = STATE::RESULT_STATE;	// ���� ����
	bool			r_sizeflag = false;

	int				recvbytes = 0;
	int				comp_recvbytes = 0;
	int				sendbytes = 0;
	int				comp_sendbytes = 0;

	char			recvbuf[BUFSIZE] = { 0 };
	char			sendbuf[BUFSIZE] = { 0 };

	WSABUF			r_wsabuf = {};
	WSABUF			s_wsabuf = {};

	class Board*	board = nullptr;				// ���������� ������ 1:1����
	class Room*		room = nullptr;					// ��� ������ N:1 ����
	string			name = "";						// ������ �̸�


	int				id = -1;						// ���� ������ ���̵�

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

	// ��������� ���ؼ� �Ƚ���ϴ�.
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