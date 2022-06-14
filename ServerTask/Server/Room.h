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

	void AddUser(SocketInfo* p_info);			// 유저가 추가되었습니다.
	void DelUser(SocketInfo* p_info);			// 유저가 방에서 나갔으므로 룸을 지웁니다.
	const vector<SocketInfo*>& GetUser() { return _vecUser; }		// 순회를 위한 프로퍼티

	int UserCount();							// 현재 유저 수
private:
	string					_name = "";			// 룸에 이름
	vector<SocketInfo*>		_vecUser;			// 유저 정보
	Lock					_lock;				// 유저 정보가 바뀔때 사용하는 녀석
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
	void Init(int size = 5);				// 방을 만들기 위한 초기작업
	void End();								// 룸을 삭제합니다.

	int RoomList(PROTOCOL protocol, char* buffer);		// 룸 리스트에 대한 정보를 버퍼에 적습니다.

	bool AddUser(const string& p_roomName, SocketInfo* p_info);		// 유저가 추가됬을때
	Room* FindRoom(const string& p_roomName);						// 룸의 정보를 찾을때

	// 생성
	void AddRoom(string p_name);			// 룸이 추가되었을때

	// 삭제
	Room* DelRoom(int index);				// 룸을 삭제했을때
	Room* DelRoom(string p_name);			// 룸을 삭제했을때

private:
	vector<Room*> _vecRoom;					// 룸의 대한 정보들
	Lock _lock;								// 룸매니저 전용 락
};
