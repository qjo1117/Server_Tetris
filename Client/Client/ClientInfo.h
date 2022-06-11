#pragma once


// Unordered_map�� �̿��ؼ� ���� ����
enum class PROTOCOL
{
	NONE = -1,
	C_CONNECT = 100,			// Ŭ�� �����ϸ� ù��°�� �����ش�.
	C_START,					// ������ �����մϴ�.
	C_EXIT,						// �������ϴ�.

	C_DIR_LEFT,
	C_DIR_RIGHT,
	C_DIR_DOWN,

	C_DIR_CHANGE,
	C_DIR_SPACE,

	// ----------------------------------------------

	S_START,						// 
	S_GAME_START,					// ������ �����մϴ�.
	S_GAME_BOARD_INFO,				// ������ ������ ���� �����մϴ�.
	S_GAME_BOARD_LINE_CLEAR,		// ���� ������ Ŭ����Ǵ� ������ �����մϴ�.
	S_GAME_FAILED,					// ���ӿ��� �����ϴ�.
	
	

	EXIT,						// ���Ż��
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

