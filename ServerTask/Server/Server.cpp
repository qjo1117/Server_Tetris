
#include "pch.h"
#include "Board.h"
#include "Room.h"


// 작업자 스레드 함수
DWORD WINAPI WorkerThread(LPVOID arg);

// 오류 출력 함수
void err_quit(char* msg);
void err_display(char* msg);

/* --------------- 편의 함수  --------------- */
bool Recv(SocketInfo* _ptr);
int CompleteRecv(SocketInfo* _ptr, int _completebyte);
bool Send(SocketInfo* _ptr, int _size);
int CompleteSend(SocketInfo* _ptr, int _completebyte);
void CompleteRecvProcess(SocketInfo* _ptr);

/* --------------- 정의한 함수  --------------- */
void ExitProcess(SocketInfo* p_ptr);								// 종료를 실행하는 함수
int RoomProcess(SocketInfo* p_ptr, char* p_recvPtr);				// 룸 리스트를 전달하는 함수
void RoomCreateProcess(SocketInfo* p_ptr, char* p_recvPtr);			// 룸을 만드는 함수
void GameWaitProcess(char* p_roomName);								// 게임 기다리라고 하는 함수
void DirectionProcess(SocketInfo* p_ptr, char* p_sendPtr);			// 키보드입력에 따른 함수
void BlockDownProcess(SocketInfo* p_ptr, char* p_recvPtr);			// 블록이 자동으로 내려가는 함수

void SendBoard(SocketInfo* p_ptr, STATE p_state);					// 보드에 대한 정보를 전달하는 함수
void DelListUser(SocketInfo* p_user);								// 유저를 삭제하는 함수


/* --------------- 변수 --------------- */

// Main Server Cpp 전용 크리티컬 섹션
Lock GLock;

// 프로토콜 실행할 녀석
unordered_map<PROTOCOL, function<void()>> ProcessFunc;

// 유저를 담아두는 녀석
vector<SocketInfo*> UserList;

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
	retval = ::bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
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

		// Accpet일 경우 
		if ((iocpEvent->type == EventType::Accept && user == nullptr) == false) {
			// 클라이언트 정보 얻기
			// 비동기 입출력 결과 확인
			if (retval == 0 || cbTransferred == 0) {
 				if (retval == 0) {
					DWORD temp1, temp2;
					WSAGetOverlappedResult(user->sock, (LPWSAOVERLAPPED)&iocpEvent,
						&temp1, FALSE, &temp2);
					err_display((char*)"WSAGetOverlappedResult()");
				}

				// 락을 잡고 유저나갔을때 할만한 짓들을 해준다.
				if (user) {
					// 메모리를 건드는 작업이니 락을 잡습니다.
					lock_guard<Lock> lockGuard(GLock);
					
					// 어떤 유저가 나갔는지 알려줍니다.
					SOCKADDR_IN clientaddr;
					int addrlen = sizeof(clientaddr);
					getpeername(user->sock, (SOCKADDR*)&clientaddr, &addrlen);
					printf("\n[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
						inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

					// 연결끊고
					closesocket(user->sock);

					// 룸에 존재하는 경우 종료
					if (user->room) {
						user->room->DelUser(user);
					}
					
					// 리스트에서도 없애줍니다.
					DelListUser(user);

					// 유저 삭제
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
			{
				// 돟기화 작업을 해준다.
				lock_guard<Lock> lockGuard(GLock);
				SocketInfo* ptr = new SocketInfo;
				if (ptr == NULL) break;
				ZeroMemory(&ptr->r_overlapped, sizeof(OVERLAPPED));
				ptr->sock = client_sock;
				ptr->recvbytes = 0;
				ptr->sendbytes = 0;
				ptr->r_wsabuf.buf = ptr->recvbuf;
				ptr->r_wsabuf.len = ptr->recvbytes;

				ptr->board = new Board;

				UserList.push_back(ptr);

				SOCKADDR_IN clientaddr;
				int addrlen = sizeof(clientaddr);
				getpeername(ptr->sock, (SOCKADDR*)&clientaddr, &addrlen);
				printf("[Computer] Connect: IP : %s, PORT : %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

				// 비동기 입출력 시작
				if (!Recv(ptr)) {
					ptr->state = DISCONNECTED_STATE;
				}

				delete iocpEvent;
			}

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
				printf("Send Error\n");
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
	printf("[%s] %s", msg, (LPTSTR)lpMsgBuf);
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
	if (_ptr->comp_sendbytes >= _ptr->sendbytes) {
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
	UnPacking(&recvPtr, &protocol);

	int size = 0;

	switch (protocol) {
	case PROTOCOL::C_ROOM:
		// 룸에 접속했으면 룸에 대한 리스트를 셋팅해준다.
		_ptr->state = STATE::ROOM_STATE;
		size = RoomManager::GetI()->RoomList(PROTOCOL::S_ROOM, _ptr->sendbuf);
		break;

	case PROTOCOL::C_ROOM_CREATE:
		// 룸을 만들어준다.
		RoomCreateProcess(_ptr, recvPtr);
		break;

	case PROTOCOL::C_START:
		// 룸에서 접속합니다.
		size = RoomProcess(_ptr, recvPtr);
		break;

	case PROTOCOL::C_DIR:
		// 키보드 입력
		DirectionProcess(_ptr, recvPtr);
		break;

	case PROTOCOL::C_BLOCK_DOWN:
		// 블록 다운
		BlockDownProcess(_ptr, recvPtr);
		break;

	case PROTOCOL::C_GAME_WAIT:
		// 게임을 기다릴때 필요한 정보를 실행합니다.
		GameWaitProcess(const_cast<char*>(&_ptr->room->GetName()[0]));
		break;

	case PROTOCOL::EXIT:
	case PROTOCOL::C_EXIT:
		ExitProcess(_ptr);

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

void ExitProcess(SocketInfo* p_ptr)
{
	// 버퍼를 건드는 작업이기때문에 락을 걸어준다.
	lock_guard<Lock> lockGaurd(GLock);

	// 유저는 나갔다고 전달해준다.
	char* sendPtr = p_ptr->sendbuf + sizeof(int);
	int size = Packing(&sendPtr, PROTOCOL::EXIT);
	memcpy(p_ptr->sendbuf, &size, sizeof(int));
	size += sizeof(int);

	if (!Send(p_ptr, size)) {
		p_ptr->state = DISCONNECTED_STATE;
	}
	
	// 현재 접속중임 룸이 존재할 경우 상대방을 강퇴시킨다.
	Room* room = p_ptr->room;
	if (room && room->GetUser().size() > 1) {
		// 만약 인게임중일때
		int otherId = (p_ptr->id + 1) % 2;
		SocketInfo* other = p_ptr->room->GetUser()[otherId];		// 다른 유저를 찾고

		// 유저에게 인게임에서 기다리라고 한다.
		sendPtr = other->sendbuf + sizeof(int);
		size = Packing(&sendPtr, PROTOCOL::S_GAME_WAIT);
		memcpy(other->sendbuf, &size, sizeof(int));
		size += sizeof(int);

		if (!Send(other, size)) {
			other->state = DISCONNECTED_STATE;
		}

		// 그리고 룸에 해당 유저에대한 정보를 지워준다.
		other->room->DelUser(p_ptr); 
		p_ptr->room = nullptr;
	}
}

int RoomProcess(SocketInfo* p_ptr, char* p_recvPtr)
{
	// 버퍼를 건드는 작업이기때문에 락을 걸어준다.
	lock_guard<Lock> lockGaurd(GLock);
	char buffer[BUFSIZE] = { 0 };
	UnPacking(&p_recvPtr, buffer);

	int size = 0;
	
	// 접속이 가능하다면
	if (RoomManager::GetI()->AddUser(buffer, p_ptr) == true) {
		// 룸에 유저가 추가된다
		char name[BUFSIZE] = { 0 };
		UnPacking(&p_recvPtr, name);
		p_ptr->name = name;

		// 게임 기다리는 상태로 돌입한다.
		char* sendPtr = p_ptr->sendbuf + sizeof(int);
		size = Packing(&sendPtr, PROTOCOL::S_GAME_WAIT);
		::memcpy(p_ptr->sendbuf, &size, sizeof(int));
		size += sizeof(int);

		// 보드가 실행가능하다고 알려준다.
		p_ptr->board->SetBoard(true);
	}
	else {
		// 룸에 유저 추가가 안된다.
		size = RoomManager::GetI()->RoomList(PROTOCOL::S_ROOM, p_ptr->sendbuf);
	}

	return size;
}

void RoomCreateProcess(SocketInfo* p_ptr, char* p_recvPtr)
{
	// 버퍼를 건드는 작업이기때문에 락을 걸어준다.
	lock_guard<Lock> lockGaurd(GLock);
	char buffer[64] = { 0 };

	// 룸에 대한 이름을 얻어옵니다.
	UnPacking(&p_recvPtr, buffer);

	// 룸매니저에서 룸을 추가합니다.
	RoomManager::GetI()->AddRoom(buffer);

	int size = UserList.size();

	// 해당 유저중 룸상태인 유저들에게 갱신 정보를 전달합니다.
	for (int i = 0; i < size; ++i) {
		SocketInfo* user = UserList[i];
		if (user->state == STATE::ROOM_STATE) {
			int size = RoomManager::GetI()->RoomList(PROTOCOL::S_ROOM_UPDATE, user->sendbuf);
			if (!Send(user, size)) {
				user->state = DISCONNECTED_STATE;
			}
		}
	}
}

void GameWaitProcess(char* p_roomName)
{
	// 해당하는 룸 얻어오기 위해 룸을 가져옵니다.
	Room* room = RoomManager::GetI()->FindRoom(p_roomName);

	// 버퍼를 건드는 작업이기때문에 락을 걸어준다.
	lock_guard<Lock> lockGuard(GLock);
	int size = 0;
	size = UserList.size();

	// 룸에 대한 정보가 갱신되었으므로 다른 유저들에게 알려줍니다.
	for (int i = 0; i < size; ++i) {
		SocketInfo* user = UserList[i];
		if (user->state == STATE::ROOM_STATE) {
			int size = RoomManager::GetI()->RoomList(PROTOCOL::S_ROOM_UPDATE, user->sendbuf);
			if (!Send(user, size)) {
				user->state = DISCONNECTED_STATE;
			}
		}
	}

	// 유저가 풀방이 아닐경우 끝냅니다.
	if (room->UserCount() != 2) {
		return;
	}

	// 풀방이 되었으면 게임을 시작합니다.
	size = 0;
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

void DirectionProcess(SocketInfo* p_ptr, char* p_recvPtr)
{
	lock_guard<Lock> lockGuard(GLock);
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

	// 키가 입력되었다면 내용을 전달해줍니다.
	SendBoard(p_ptr, STATE::MOVE_GAME_STATE);
}


void BlockDownProcess(SocketInfo* p_ptr, char* p_recvPtr)
{
	p_ptr->board->MovePos(0, 1);

	// 키가 입력되었다면 내용을 전달해줍니다.
	SendBoard(p_ptr, STATE::AUTO_MOVE_GAME_STATE);
}

void SendBoard(SocketInfo* p_ptr, STATE p_state)
{
	lock_guard<Lock> lockGuard(GLock);

	int size = 0;
	if (p_ptr->board->IsBoard()) {
		// 일반적인 상황 보드판에 정보를 상대 자신에게 알려준다.
		p_ptr->state = p_state;

		size = p_ptr->board->SendPack(p_ptr, 0, p_ptr->name);
		if (!Send(p_ptr, size)) {
			p_ptr->state = DISCONNECTED_STATE;
		}

		if (p_ptr->room == nullptr) {
			return;
		}

		int otherId = (p_ptr->id + 1) % 2;
		SocketInfo* other = p_ptr->room->GetUser()[otherId];
		size = p_ptr->board->SendPack(other, 1, p_ptr->name);
		if (!Send(other, size)) {
			other->state = DISCONNECTED_STATE;
		}
	}
	else {
		// 보드판 실행이 실패되었을경우 게임에서 졌다고 판단한다.
		char* sendPtr = nullptr;

		// 해당하는 녀석은 게임에서 졌다고 알려주기
		sendPtr = p_ptr->sendbuf + sizeof(int);
		size = Packing(&sendPtr, PROTOCOL::S_GAME_FAILED);
		::memcpy(p_ptr->sendbuf, &size, sizeof(int));
		size += sizeof(int);
		if (!Send(p_ptr, size)) {
			p_ptr->state = DISCONNECTED_STATE;
		}

		if (p_ptr->room == nullptr) {
			return;
		}

		// 해당하는 녀석은 게임에서 이겼다고 알려주기
		int otherId = (p_ptr->id + 1) % 2;
		SocketInfo* other = p_ptr->room->GetUser()[otherId];
		sendPtr = other->sendbuf + sizeof(int);
		size = Packing(&sendPtr, PROTOCOL::S_GAME_WIN);
		::memcpy(other->sendbuf, &size, sizeof(int));
		size += sizeof(int);
		if (!Send(other, size)) {
			other->state = DISCONNECTED_STATE;
		}

		// 룸에서 나간다.
		other->room->DelUser(p_ptr);
		other->room->DelUser(other);

		p_ptr->room = nullptr;
		other->room = nullptr;

		p_ptr->state = STATE::RESULT_STATE;
		other->state = STATE::RESULT_STATE;

		size = UserList.size();
		for (int i = 0; i < size; ++i) {
			SocketInfo* user = UserList[i];
			if (user->state == STATE::ROOM_STATE) {
				int size = RoomManager::GetI()->RoomList(PROTOCOL::S_ROOM_UPDATE, user->sendbuf);
				if (!Send(user, size)) {
					user->state = DISCONNECTED_STATE;
				}
			}
		}
	}
}

void DelListUser(SocketInfo* p_user)
{
	// 락을 새롭게 잡아주고 Read or Write를 실행합니다.
	lock_guard<Lock> lockGuard(GLock);
	auto iter = find(UserList.begin(), UserList.end(), p_user);
	if (iter == UserList.end()) {
		return;
	}

	UserList.erase(iter);
}
