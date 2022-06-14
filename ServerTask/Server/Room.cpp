#include "pch.h"
#include "Room.h"
#include "Board.h"

void Room::AddUser(SocketInfo* p_info)
{
	lock_guard<Lock> lockGuard(_lock);
	if (p_info->room) {
		printf("�̳� �� ���ִ°��� %s\n", p_info->name.data());
		return;
	}

	// ���� �ڽ��� � �뿡 �����ִ��� �˾Ƴ���.
	p_info->room = this;
	p_info->id = _vecUser.size();
	_vecUser.push_back(p_info);
}

void Room::DelUser(SocketInfo* p_info)
{
	lock_guard<Lock> lockGuard(_lock);
	auto iter = find(_vecUser.begin(), _vecUser.end(), p_info);
	if (iter == _vecUser.end()) {
		return;
	}

	(*iter)->board->Init();
	_vecUser.erase(iter);
}

int Room::UserCount()
{
	return _vecUser.size();
}

/* -------------------------- RoomManager -------------------------- */

void RoomManager::Init(int size)
{
	AddRoom("Tetris Room");					// ��Ʈ������ ��������ϴ�.
	AddRoom("Professional Best Room");		// ������ ¯¯
	AddRoom("I hate API");					// LoadBitmap�ϸ� �ȿ�����
	AddRoom("I like Many Score");			// ���� ���� �ֽø� �����ϰڽ��ϴ�.
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

int RoomManager::RoomList(PROTOCOL protocol, char* buffer)
{
	char* ptr = buffer + sizeof(int);

	// ���� �� ����Ʈ�� �����ش�.
	int size = _vecRoom.size();
	int packSize = Packing(&ptr, protocol, size);
	
	for (auto room : _vecRoom) { 
		// �� �̸�, ���� ��
		packSize += Packing(&ptr, &room->GetName()[0], room->UserCount());
	}

	// ������ ����� �������ش�.
	::memcpy(buffer, &packSize, sizeof(int));
	packSize += sizeof(int);

	return packSize;
}

bool RoomManager::AddUser(const string& p_roomName, SocketInfo* p_info)
{
	Room* room = FindRoom(p_roomName);
	
	// ���� ��� �ȵ�
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

