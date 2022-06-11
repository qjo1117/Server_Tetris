#pragma once


// Unordered_map을 이용해서 만들 예정
enum class PROTOCOL
{
	NONE = -1,
	C_CONNECT = 100,			// 클라가 접속하면 첫번째로 보내준다.
	C_START,					// 게임을 시작합니다.
	C_EXIT,						// 나갔습니다.

	C_DIR_LEFT,
	C_DIR_RIGHT,
	C_DIR_DOWN,

	C_DIR_CHANGE,
	C_DIR_SPACE,

	// ----------------------------------------------

	S_START,						// 
	S_GAME_START,					// 게임을 시작합니다.
	S_GAME_BOARD_INFO,				// 보드의 정보를 전부 전달합니다.
	S_GAME_BOARD_LINE_CLEAR,		// 보드 라인이 클리어되는 정보를 전달합니다.
	S_GAME_FAILED,					// 게임에서 졌습니다.
	
	

	EXIT,						// 긴급탈출
	END,
};


/*---------------
	BoardInfo
----------------*/
struct BoardInfo
{
public:
	int32 board[10][17];
};

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
};

