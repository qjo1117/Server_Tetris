#pragma once

#include <string>
#include <vector>

/* -----------------
*		Room
----------------- */
class Room
{
public:
	const string& GetName() { return _name; }
	void SetName(const string& p_name) { _name = p_name; }

	void AddUser(SocketInfo* p_info);			// ������ �߰��Ǿ����ϴ�.
	void DelUser(SocketInfo* p_info);			// ������ �濡�� �������Ƿ� ���� ����ϴ�.
	const vector<SocketInfo*>& GetUser() { return _vecUser; }		// ��ȸ�� ���� ������Ƽ

	int UserCount();							// ���� ���� ��
private:
	string					_name = "";			// �뿡 �̸�
	vector<SocketInfo*>		_vecUser;			// ���� ����
	Lock					_lock;				// ���� ������ �ٲ� ����ϴ� �༮
};


/* -----------------
*	 RoomManager
----------------- */
class RoomManager
{
public:
	/* ------- Single Ton ------- */
	static RoomManager* GetI() {
		static RoomManager instance;
		return &instance;
	}
	RoomManager() {}
	~RoomManager() {
		for (Room* room : _vecRoom) {
			if (room) {
				delete room;
			}
			room = nullptr;
		}
	}

public:
	void Init(int size = 5);				// ���� ����� ���� �ʱ��۾�
	void End();								// ���� �����մϴ�.

	int RoomList(PROTOCOL protocol, char* buffer);		// �� ����Ʈ�� ���� ������ ���ۿ� �����ϴ�.

	bool AddUser(const string& p_roomName, SocketInfo* p_info);		// ������ �߰�������
	Room* FindRoom(const string& p_roomName);						// ���� ������ ã����

	// ����
	void AddRoom(string p_name);			// ���� �߰��Ǿ�����

	// ����
	Room* DelRoom(int index);				// ���� ����������
	Room* DelRoom(string p_name);			// ���� ����������

private:
	vector<Room*> _vecRoom;					// ���� ���� ������
	Lock _lock;								// ��Ŵ��� ���� ��
};
