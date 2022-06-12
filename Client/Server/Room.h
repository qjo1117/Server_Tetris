#pragma once

#include <string>
#include <vector>

/* -----------------
*		Room
----------------- */
class Room
{
public:

public:

	const string& GetName() { return _name; }
	void SetName(const string& p_name) { _name = p_name; }

	void AddUser(SocketInfo* p_info);
	vector<SocketInfo*>& GetUser() { return _vecUser; }

	int UserCount();
private:
	string _name = "";
	vector<SocketInfo*> _vecUser;
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
	void Init(int size = 5);
	void End();

	int RoomList(char* buffer);

	bool AddUser(const string& p_roomName, SocketInfo* p_info);
	Room* FindRoom(const string& p_roomName);

	// 생성
	void AddRoom(string p_name);

	// 삭제
	Room* DelRoom(int index);
	Room* DelRoom(string p_name);

private:
	vector<Room*> _vecRoom;
	Lock _lock;
};
