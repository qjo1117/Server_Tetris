
#include "pch.h"
#include "Board.h"
#include "Room.h"


// 작업자 스레드 함수
DWORD WINAPI WorkerThread(LPVOID arg);
// 오류 출력 함수
void err_quit(char* msg);
void err_display(char* msg);

bool Recv(SocketInfo* _ptr);
int CompleteRecv(SocketInfo* _ptr, int _completebyte);

bool Send(SocketInfo* _ptr, int _size);
int CompleteSend(SocketInfo* _ptr, int _completebyte);

void CompleteRecvProcess(SocketInfo* _ptr);


int SelectOrReadRoomProcess(SocketInfo* p_ptr, char* p_recvPtr);
void BroadCastRoomStart(char* p_roomName);

void DirProcess(SocketInfo* p_ptr, char* p_sendPtr);
void BlockDownProcess(SocketInfo* p_ptr, char* p_recvPtr);


// Main Server Cpp 전용 크리티컬 섹션
Lock GLock;

int main(int argc, char* argv[])
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

	// 입출력 완료 포트 생성
	HANDLE hcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (hcp == NULL) return 1;

	// CPU 개수 확인
	SYSTEM_INFO si;
	GetSystemInfo(&si);

	// (CPU 개수 * 2)개의 작업자 스레드 생성
	HANDLE hThread;
	for (int i = 0; i < (int)si.dwNumberOfProcessors * 2; i++) {
		hThread = CreateThread(NULL, 0, WorkerThread, hcp, 0, NULL);
		if (hThread == NULL) return 1;
		CloseHandle(hThread);
	}

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit((char*)"socket()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit((char*)"bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit((char*)"listen()");

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	DWORD recvbytes, flags;

	RoomManager::GetI()->Init();

	while (1) {
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			err_display((char*)"accept()");
			break;
		}


		// 소켓과 입출력 완료 포트 연결
		IocpEvent* aceepEvent = new IocpEvent;
		ZeroMemory(aceepEvent, sizeof(OVERLAPPED));
		aceepEvent->Init(EventType::Accept);
		aceepEvent->owner = nullptr;
		PostQueuedCompletionStatus(hcp, -1, client_sock, (LPOVERLAPPED)aceepEvent);
	}

	// 윈속 종료
	WSACleanup();
	return 0;
}

// 작업자 스레드 함수
DWORD WINAPI WorkerThread(LPVOID arg)
{
	int retval;
	HANDLE hcp = (HANDLE)arg;

	while (1) {
		// 비동기 입출력 완료 기다리기
		DWORD cbTransferred;
		SOCKET client_sock;
		IocpEvent* iocpEvent = nullptr;
		SocketInfo* user = nullptr;
		retval = GetQueuedCompletionStatus(hcp, &cbTransferred,
			&client_sock, (LPOVERLAPPED*)&iocpEvent, INFINITE);

		user = iocpEvent->owner;

		if ((user == nullptr && iocpEvent->type == EventType::Accept) == false) {
			// 클라이언트 정보 얻기
			// 비동기 입출력 결과 확인
			if (retval == 0 || cbTransferred == 0) {
 				if (retval == 0) {
					DWORD temp1, temp2;
					WSAGetOverlappedResult(user->sock, (LPWSAOVERLAPPED)&iocpEvent,
						&temp1, FALSE, &temp2);
					err_display((char*)"WSAGetOverlappedResult()");
				}

				if (user) {
					SOCKADDR_IN clientaddr;
					int addrlen = sizeof(clientaddr);
					getpeername(user->sock, (SOCKADDR*)&clientaddr, &addrlen);
					printf("\n[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
						inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

					closesocket(user->sock);

					delete user;
				}
				user = nullptr;

				continue;
			}
		}

		int result = 0;
		switch (iocpEvent->type) {
		case EventType::Accept: {
			DWORD recvbytes = 0, flags = 0;
			CreateIoCompletionPort((HANDLE)client_sock, hcp, client_sock, 0);
			// 소켓 정보 구조체 할당
			SocketInfo* ptr = new SocketInfo;
			if (ptr == NULL) break;
			ZeroMemory(&ptr->r_overlapped, sizeof(OVERLAPPED));
			ptr->sock = client_sock;
			ptr->recvbytes = 0;
			ptr->sendbytes = 0;
			ptr->r_wsabuf.buf = ptr->recvbuf;
			ptr->r_wsabuf.len = ptr->recvbytes;

			ptr->board = new Board;

			printf("[Computer] Connect: IP : %s, PORT : %d\n", inet_ntoa(ptr->addr.sin_addr), ntohs(ptr->addr.sin_port));

			// 비동기 입출력 시작
			if (!Recv(ptr)) {
				user->state = DISCONNECTED_STATE;
			}

			delete iocpEvent;
		}
			break;

		case EventType::Recv:
			result = CompleteRecv(user, cbTransferred);
			switch (result) {
			case SOC_ERROR:
				printf("Recv Error");
				return 0;
			case SOC_FALSE:
				continue;
			case SOC_TRUE:
				break;
			}

			CompleteRecvProcess(user);

			if (!Recv(user)) {
				user->state = DISCONNECTED_STATE;
			}
			break;

		case EventType::Send:
			result = CompleteSend(user, cbTransferred);
			switch (result) {
			case SOC_ERROR:
				printf("Send Error");
				return 0;
			case SOC_FALSE:
				continue;
			case SOC_TRUE:
				break;
			}

			break;
		}
	}

	return 0;
}

// 소켓 함수 오류 출력 후 종료
void err_quit(char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPCWSTR)msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력
void err_display(char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}


bool Recv(SocketInfo* _ptr)
{
	int retval;
	DWORD recvbytes;
	DWORD flags = 0;

	ZeroMemory(&_ptr->r_overlapped, sizeof(OVERLAPPED));

	_ptr->r_wsabuf.buf = _ptr->recvbuf + _ptr->comp_recvbytes;

	if (_ptr->r_sizeflag) {
		_ptr->r_wsabuf.len = _ptr->recvbytes - _ptr->comp_recvbytes;
	}
	else {
		_ptr->recvbytes = sizeof(int) - _ptr->comp_recvbytes;
		_ptr->r_wsabuf.len = _ptr->recvbytes;
	}

	retval = WSARecv(_ptr->sock, &_ptr->r_wsabuf, 1, &recvbytes,
		&flags, &_ptr->r_overlapped, nullptr);
	if (retval == SOCKET_ERROR) {
		if (WSAGetLastError() != WSA_IO_PENDING) {
			err_display((char*)"WSARecv()");
			return false;
		}
	}

	return true;
}

int CompleteRecv(SocketInfo* _ptr, int _completebyte)
{
	_ptr->comp_recvbytes += _completebyte;

	if (_ptr->comp_recvbytes == _ptr->recvbytes) {
		if (!_ptr->r_sizeflag) {
			memcpy(&_ptr->recvbytes, _ptr->recvbuf, sizeof(int));
			_ptr->comp_recvbytes = 0;
			_ptr->r_sizeflag = true;

			if (!Recv(_ptr))
			{
				return SOC_ERROR;
			}

			return SOC_FALSE;
		}

		_ptr->comp_recvbytes = 0;
		_ptr->recvbytes = 0;
		_ptr->r_sizeflag = false;

		return SOC_TRUE;
	}
	else {
		if (!Recv(_ptr)) {
			return SOC_ERROR;
		}

		return SOC_FALSE;
	}
}

bool Send(SocketInfo* _ptr, int _size)
{
	int retval;
	DWORD sendbytes;
	DWORD flags;

	ZeroMemory((OVERLAPPED*)&_ptr->s_overlapped, sizeof(OVERLAPPED));
	if (_ptr->sendbytes == 0) {
		_ptr->sendbytes = _size;
	}

	_ptr->s_wsabuf.buf = _ptr->sendbuf + _ptr->comp_sendbytes;
	_ptr->s_wsabuf.len = _ptr->sendbytes - _ptr->comp_sendbytes;

	retval = WSASend(_ptr->sock, &_ptr->s_wsabuf, 1, &sendbytes,
		0, &_ptr->s_overlapped, nullptr);
	if (retval == SOCKET_ERROR) {
		if (WSAGetLastError() != WSA_IO_PENDING) {
			err_display((char*)"WSASend()");
			//RemoveClientInfo(_ptr);
			return false;
		}
	}

	return true;
}

int CompleteSend(SocketInfo* _ptr, int _completebyte)
{
	_ptr->comp_sendbytes += _completebyte;
	if (_ptr->comp_sendbytes == _ptr->sendbytes) {
		_ptr->comp_sendbytes = 0;
		_ptr->sendbytes = 0;

		return SOC_TRUE;
	}
	if (!Send(_ptr, _ptr->sendbytes)) {
		return SOC_ERROR;
	}

	return SOC_FALSE;
}

void CompleteRecvProcess(SocketInfo* _ptr)
{
	char msg[BUFSIZE] = { 0 };

	PROTOCOL protocol = PROTOCOL::NONE;

	char* recvPtr = _ptr->recvbuf;

	ZeroMemory(_ptr->sendbuf, sizeof(_ptr->sendbuf));
	char* sendPtr = _ptr->sendbuf + sizeof(int);
	UnPacking(&recvPtr, &protocol);

	int size = 0;

	//size = Packing(&sendPtr, PROTOCOL::READ, );
	//memcpy(_ptr->sendbuf, &size, sizeof(int));
	//size += sizeof(int);

	volatile static int count = 0;
	count += 1;

	switch (protocol) {
	case PROTOCOL::C_CONNECT: 

		break;

	case PROTOCOL::C_ROOM:
		size = RoomManager::GetI()->RoomList(_ptr->sendbuf);
		break;

	case PROTOCOL::C_START:
		size = SelectOrReadRoomProcess(_ptr, recvPtr);
		break;

	case PROTOCOL::C_DIR:
		DirProcess(_ptr, recvPtr);
		break;

	case PROTOCOL::C_BLOCK_DOWN:
		BlockDownProcess(_ptr, recvPtr);
		break;

	case PROTOCOL::C_GAME_WAIT:
		BroadCastRoomStart(const_cast<char*>(&_ptr->room->GetName()[0]));
		break;

	case PROTOCOL::EXIT:
	case PROTOCOL::C_EXIT:
		// TODO : 유저가 중간에 나갔을대 룸에 있을 경우 룸을 나가자고 하자.

		size = Packing(&sendPtr, PROTOCOL::EXIT);
		memcpy(_ptr->sendbuf, &size, sizeof(int));
		size += sizeof(int);
		_ptr->state = DISCONNECTED_STATE;
		break;
	}

	// Size 0이면 아무것도 보내지않는다.
	if (size == 0) {
		return;
	}

	if (!Send(_ptr, size)) {
		_ptr->state = DISCONNECTED_STATE;
	}
}

int SelectOrReadRoomProcess(SocketInfo* p_ptr, char* p_recvPtr)
{
	char buffer[BUFSIZE] = { 0 };
	UnPacking(&p_recvPtr, buffer);

	int size = 0;
	if (RoomManager::GetI()->AddUser(buffer, p_ptr) == true) {
		// 룸에 유저가 추가된다
		char* sendPtr = p_ptr->sendbuf + sizeof(int);
		size = Packing(&sendPtr, PROTOCOL::S_GAME_WAIT);
		::memcpy(p_ptr->sendbuf, &size, sizeof(int));
		size += sizeof(int);
	}
	else {
		// 룸에 유저 추가가 안된다.
		size = RoomManager::GetI()->RoomList(p_ptr->sendbuf);
	}

	return size;
}

void BroadCastRoomStart(char* p_roomName)
{
	Room* room = RoomManager::GetI()->FindRoom(p_roomName);
	if (room->UserCount() != 2) {
		return;
	}

	int size = 0;
	for (SocketInfo* user : room->GetUser()) {
		char* sendPtr = user->sendbuf + sizeof(int);
		size = Packing(&sendPtr, PROTOCOL::S_START);
		::memcpy(user->sendbuf, &size, sizeof(int));
		size += sizeof(int);
		if (!Send(user, size)) {
			user->state = DISCONNECTED_STATE;
		}
	}
}

void DirProcess(SocketInfo* p_ptr, char* p_recvPtr)
{
	// 방향을 얻어냅니다.
	int dir = 0;
	UnPacking(&p_recvPtr, &dir);

	// 각 키에 맞게 움직입니다.
	switch (dir) {
	case RIGHT:
		p_ptr->board->MovePos(1, 0);
		break;

	case LEFT:
		p_ptr->board->MovePos(-1, 0);
		break;

	case DOWN:
		p_ptr->board->MovePos(0, 1);
		break;

	case SPACE:
		p_ptr->board->SpaceBar();
		break;

	case CHANGE:
		p_ptr->board->ChangeBlock();
		break;
	}

	// Send Packet
	// TODO : 
	int size = 0;

	{
		lock_guard<Lock> lockGuard(GLock);
		size = p_ptr->board->SendPack(p_ptr, 0);
		if (!Send(p_ptr, size)) {
			p_ptr->state = DISCONNECTED_STATE;
		}
	
		int otherId = (p_ptr->id + 1) % 2;
		SocketInfo* other = p_ptr->room->GetUser()[otherId];
		size = p_ptr->board->SendPack(other, 1);
		if (!Send(other, size)) {
			other->state = DISCONNECTED_STATE;
		}
	}
	
}


void BlockDownProcess(SocketInfo* p_ptr, char* p_recvPtr)
{
	int size = 0;
	p_ptr->board->MovePos(0, 1);

	{
		lock_guard<Lock> lockGuard(GLock);
		size = p_ptr->board->SendPack(p_ptr, 0);
		if (!Send(p_ptr, size)) {
			p_ptr->state = DISCONNECTED_STATE;
		}

		int otherId = (p_ptr->id + 1) % 2;
		SocketInfo* other = p_ptr->room->GetUser()[otherId];
		size = p_ptr->board->SendPack(other, 1);
		if (!Send(other, size)) {
			other->state = DISCONNECTED_STATE;
		}
	}
}