#include "pch.h"
#include "Room.h"

void Room::AddUser(SocketInfo* p_info)
{
	if (p_info->room) {
		printf("이놈 왜 룸있는거지 %s\n", p_info->name.data());
		return;
	}

	// 현재 자신이 어떤 룸에 속해있는지 알아낸다.
	p_info->room = this;
	p_info->id = _vecUser.size();
	_vecUser.push_back(p_info);
}

int Room::UserCount()
{
	return _vecUser.size();
}


void RoomManager::Init(int size)
{
	string roomName = "Room ";

	for (int i = 0; i < size; ++i) {
		AddRoom(roomName + to_string(i));
	}

}

void RoomManager::End()
{
	lock_guard<Lock> lockGuard(_lock);
	for (auto room : _vecRoom) {
		if (room) {
			delete room;
		}
		room = nullptr;
	}
}

int RoomManager::RoomList(char* buffer)
{
	char* ptr = buffer + sizeof(int);

	// 현재 룸 리스트를 보내준다.
	int size = _vecRoom.size();
	int packSize = Packing(&ptr, PROTOCOL::S_ROOM, size);
	for (auto& room : _vecRoom) {
		packSize += Packing(&ptr, room->GetName().data());
	}

	// 마지막 헤더를 정의해준다.
	::memcpy(buffer, &packSize, sizeof(int));
	packSize += sizeof(int);

	return packSize;
}

bool RoomManager::AddUser(const string& p_roomName, SocketInfo* p_info)
{
	Room* room = FindRoom(p_roomName);
	
	// 없을 경우 안됨
	if (room == nullptr || room->UserCount() == 2) {
		return false;
	}

	room->AddUser(p_info);

	return true;
}

Room* RoomManager::FindRoom(const string& p_roomName)
{
	for (Room* room : _vecRoom) {
		if (room->GetName().compare(p_roomName) == 0) {
			return room;
		}
	}

	return nullptr;
}

void RoomManager::AddRoom(string p_name)
{
	lock_guard<Lock> lockGuard(_lock);
	Room* room = new Room();
	room->SetName(p_name);
	_vecRoom.push_back(room);
}

Room* RoomManager::DelRoom(int index)
{
	lock_guard<Lock> lockGuard(_lock);
	if (COMPARE(0 <=, index, < _vecRoom.size()) == false) {
		return _vecRoom[index];
	}
	return nullptr;
}

Room* RoomManager::DelRoom(string p_name)
{
	lock_guard<Lock> lockGuard(_lock);
	for (auto room : _vecRoom) {
		if (room->GetName() == p_name) {
			return room;
		}
	}
	return nullptr;
}

