#include "pch.h"
#include "ServerScene.h"
#include "Engine.h"

#include "Core.h"
#include "ResourcesManager.h"
#include "SoundManager.h"
#include "Texture.h"
#include "Timer.h"
#include "../Server/BlockShape.h"

void ServerScene::Start()
{
	this_thread::sleep_for(1s);

	Scene::Start();

	// IMGUI Init
	ImGui_Init();


	InitProtocol();
	LoadTexture();

	// ����� Recv�� �޴� �����带 �����Ѵ�.
	ServerConnect();
	
	_player = new BoardInfo();
	_target = new BoardInfo();

	/* --------------- �ʱ�ȭ �ܰ� --------------- */
	_player->board = vector<vector<char>>(Height, vector<char>(Width, 0));
	_player->block = vector<vector<char>>(4, vector<char>(4, 0));
	_player->nextBlock = vector<vector<char>>(4, vector<char>(4, 0));

	_target->board = vector<vector<char>>(Height, vector<char>(Width, 0));
	_target->block = vector<vector<char>>(4, vector<char>(4, 0));
	_target->nextBlock = vector<vector<char>>(4, vector<char>(4, 0));

	_player->name.resize(64);
	_target->name.resize(64);

	_roomName.resize(64);

	GET_SINGLE(SoundManager)->LoadSound("Score", false, "Score.wav");

	Core::GetI()->DestroyWindowAddFunc([self = static_pointer_cast<ServerScene>(shared_from_this())]() { 
		SendPacking(self->_info.socket, PROTOCOL::C_EXIT);
		self->EndRecvThread();
	});
}

void ServerScene::Update()
{
	Scene::Update();
	ImGui_Update();

}

void ServerScene::Render()
{
	Scene::Render();
	ImGui_Render();

}

void ServerScene::End()
{
	Scene::End();
	ImGui_End();

	// Ȥ�ó��ϴ� �����尡 ������� �ʴ� ������ üũ
	if (_recvThread.joinable()) {
		_recvThread.join();
	}

	closesocket(_info.socket);

	if (_player) {
		delete _player;
	}
	_player = nullptr;

	if (_target) {
		delete _target;
	}
	_target = nullptr;
}

void ServerScene::ServerConnect()
{
	int retval = 0;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		Core::loop = false;
		return;
	}

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) {
		Core::loop = false;
		return;
	}

	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) {
		Core::loop = false;
		return;
	}

	_info.socket = sock;
	_info.addr = serveraddr;

	// Recv Thread ����
	thread recvThread([self = shared_from_this()]() { static_pointer_cast<ServerScene>(self)->RecvProcess(); });
	_recvThread.swap(recvThread);
}

void ServerScene::LoadTexture()
{
	// Block Mapping
	_mapTexture[L"EmptyBlock"] = GET_SINGLE(ResourcesManager)->LoadFromTexture(L"EmptyBlock.png");

	// Player Mapping
	_mapTexture[L"Player"] = GET_SINGLE(ResourcesManager)->LoadFromTexture(L"HandlerBlock.png");

	// Menu Mapping
	_mapTexture[L"Menu"] = GET_SINGLE(ResourcesManager)->LoadFromTexture(L"Menu.png");
	_mapTexture[L"LockBlock"] = GET_SINGLE(ResourcesManager)->LoadFromTexture(L"Block.png");

	_mapTexture[L"Win"] = GET_SINGLE(ResourcesManager)->LoadFromTexture(L"Win.png");
	_mapTexture[L"Faild"] = GET_SINGLE(ResourcesManager)->LoadFromTexture(L"Faild.png");
}

void ServerScene::RecvProcess()
{
	int retVal = 0;
	// ����

	while (_isRecvProcess) {
		Recv();

		// ���� ����� ����⿡�� ���� �˹�
		_info.recvPtr = _info.recvBuffer;
		_info.sendPtr = _info.sendBuffer;

		PROTOCOL protocol = PROTOCOL::NONE;

		UnPacking(&_info.recvPtr, &protocol);
		
		// �������ݿ� �ش��ϴ� �Լ��� ȣ���Ѵ�.
		if (_mapProcess[protocol]) {
			_mapProcess[protocol]();
		}
	}

	Core::loop = false;
}

void ServerScene::InitProtocol()
{
	/* ----------- �������ݿ� ���� �Լ� ���� ----------- */

	// ������ �����ֱ⸦ ������ �Լ����� �ƴϴ�.
	// Scene�� �������������� ����ȴ�.
	_mapProcess[PROTOCOL::S_START] = [self = this]() { self->ProcessStart(); };
	_mapProcess[PROTOCOL::EXIT] = [self = this]() { self->ProcessExit(); };
	_mapProcess[PROTOCOL::S_GAME_BOARD_INFO] = [self = this]() { self->ProcessBoardInfo(); };
	_mapProcess[PROTOCOL::S_ROOM] = [self = this]() { self->ProcessRoom(); };
	_mapProcess[PROTOCOL::S_ROOM_UPDATE] = [self = this]() { self->ProcessRoomUpadte(); };
	_mapProcess[PROTOCOL::S_GAME_WAIT] = [self = this]() { self->ProcessGameWait(); };
	_mapProcess[PROTOCOL::S_GAME_WIN] = [self = this]() { self->ProcessGameWin(); };
	_mapProcess[PROTOCOL::S_GAME_FAILED] = [self = this]() { self->ProcessGameFaild(); };

	// �����ֱ⸦ �����ϸ� �̷��� �ٲٸ� �ȴ�.
	//_mapProcess[PROTOCOL::S_START] = [self = shared_from_this()]() { static_pointer_cast<ServerScene>(self)->ProcessStart(); };
}

// Recv�κ��� �� ���� �����ϴ�.
void ServerScene::Recv()
{
	// Size
	int retVal = 0;
	int size = MAX_PATH;
	retVal = recv(_info.socket, (char*)&size, sizeof(int), 0);
	if (retVal == SOCKET_ERROR || retVal == 0) {
		_isRecvProcess = false;
		Core::loop = false;
		return;
	}

	// Data
	retVal = recv(_info.socket, _info.recvBuffer, size, 0);
	if (retVal == SOCKET_ERROR || retVal == 0) {
		_isRecvProcess = false;
		Core::loop = false;
		return;
	}
}

void ServerScene::ImGui_Init()
{
	// Imgui �ʱ�ȭ �κ�
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(GET_SINGLE(Engine)->GetInfo().hWnd);
	ImGui_ImplDX9_Init(DEVICE);
}

void ServerScene::ImGui_Update()
{
	// Imgui �׸� �κ��� ���⼭ �����մϴ�.

	//  Start the Dear ImGui frame
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();


	ImGui::NewFrame();

	// ����׿�
	LogPresent();

	// �� ������Ʈ ���� �ٸ��� ����� �մϴ�.
	switch (_state) {
	case STATE::MENU:
		MenuState();
		break;

	case STATE::GAME:
		GameState_Player();
		GameState_Target();
		break;

	case STATE::ROOM:
		RoomState();
		break;

	case STATE::WIN:
	case STATE::FAILED:
		ResultState();
		break;

	default:
		// ����
		SendPacking(_info.socket, PROTOCOL::C_EXIT);
		break;
	}

	ImGui::EndFrame();
}

void ServerScene::ImGui_Render()
{
	// ���� ������� �������մϴ�.
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

void ServerScene::ImGui_End()
{
	// IMGUI ������ �κ�
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void ServerScene::MenuState()
{
	// �޴� ������Ʈ ó���� ���̴� ����Դϴ�.
	bool open = true;


	ImGui::Begin("##Menu", &open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	ImGui::SetWindowPos(ImVec2{ 150.0f,20.0f });
	// ������ �̹����� �����ͼ� ȭ�鿡 �����ݴϴ�.
	ImGui::Image(_mapTexture[L"Menu"]->GetTexture(), ImVec2{500.0f,300.0f});

	// �޴��� Line
	ImGui::SetCursorPosX(200.0f);

	// ----------- �޴� �׸��� ------------
	// {}�� �Ǿ� �մ� ������ �ڽ����� ���� �����ҷ��� �߽��ϴ�.
	{
		// Offset����ְ�
		// ��ư�� �����մϴ�.
		float buttonOffset = 14.0f;
		ImGui::BeginChild("##MenuChild", ImVec2(100.0f, 80.0f), true);
		ImGui::SetCursorPosX(buttonOffset);
		if (ImGui::Button("Start", ImVec2(70.0f, 30.0f))) {
			SendPacking(_info.socket, PROTOCOL::C_ROOM);
		}
		ImGui::SetCursorPosX(buttonOffset);
		if (ImGui::Button("Exit", ImVec2(70.0f, 30.0f))) {
			SendPacking(_info.socket, PROTOCOL::C_EXIT);
		}
		ImGui::EndChild();
	}

	ImGui::End();
}

void ServerScene::GameState_Player()
{
	// ----------- ������ ----------
	{
		bool open = true;
		ImGui::Begin("##Board_Player", &open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
		ImGui::SetWindowPos(ImVec2{ 0.0f,0.0f });
		ImGui::SetWindowSize(ImVec2{ 250.0f, -1.0f });

		// Board�� ������ �̹��� ù ��° offset
		ImVec2 baseOffset = ImVec2{ 5.0f, 10.0f };

		// ������ ������ ���� �������� �������մϴ�.
		for (int32 i = 0; i < Height; ++i) {
			for (int32 j = 0; j < Width; ++j) {
				ImGui::SetCursorPos(ImVec2{ (j * _blockSize) + baseOffset.x, (i * _blockSize) + baseOffset.y });
				
				// ���� ���� ����� ����մϴ�.
				if (_player->board[i][j] == 1) {
					ImGui::Image(_mapTexture[L"LockBlock"]->GetTexture(), ImVec2{ _blockSize, _blockSize });
				}
				else {
					ImGui::Image(_mapTexture[L"EmptyBlock"]->GetTexture(), ImVec2{ _blockSize, _blockSize });
				}
			}
		}

		// ----------- ������ ���� ������ ------------
		for (int32 i = 0; i < 4; ++i) {
			for (int32 j = 0; j < 4; ++j) {
				if (_player->block[i][j] == 1) {
					ImGui::SetCursorPos(ImVec2{ ((_player->posX + j) * _blockSize) + baseOffset.x, ((_player->posY + i) * _blockSize) + baseOffset.y });
					ImGui::Image(_mapTexture[L"Player"]->GetTexture(), ImVec2{ _blockSize, _blockSize });
				}
			}
		}


		ImGui::End();
	}

	// ----------- ������ ----------
	{
		bool open = true;
		ImGui::Begin("##NextBlock_Player", &open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
		ImGui::SetWindowPos(ImVec2{ 250.0f,0.0f });
		ImGui::SetWindowSize(ImVec2{ 140.0f, 230.0f });
		{
			// �������� ���� ����� �����ݴϴ�.
			ImGui::BeginChild("NextBlock", ImVec2{ 150.0f,100.0f });
			for (int32 i = 0; i < 4; ++i) {
				for (int32 j = 0; j < 4; ++j) {
					ImGui::SetCursorPos(ImVec2{ (j * 20.0f), (i * 20.0f) });
					if (_player->nextBlock[i][j] == 1) {
						ImGui::Image(_mapTexture[L"Player"]->GetTexture(), ImVec2{ 20.0f, 20.0f });
					}
					else {
						ImGui::Image(_mapTexture[L"EmptyBlock"]->GetTexture(), ImVec2{ 20.0f, 20.0f });
					}
				}
			}
			ImGui::EndChild();
		}


		/* --------------- UserStat ---------------- */
		{
			ImGui::BeginChild("##UserStat_Player", ImVec2{ 150.0f,80.0f });
			ImGui::Text("UserName : %s", &_player->name[0]);			// �̸��� �����ݴϴ�.
			ImGui::Text("Score : %d", _player->score);					// ������ �����ݴϴ�.
			ImGui::EndChild();
		}

		ImGui::End();
	}


	// Wait State
	if(_isWait == false) {
		// ----------- �Է¿� ���� �������� ------------

		// �� �Է¿� ���� ���������� �����մϴ�.
		if (INPUT->KeyDown("Right")) {
			SendPacking(_info.socket, PROTOCOL::C_DIR, RIGHT);
			Log(LOG_STATE::SEND, "RightButton");
		}
		if (INPUT->KeyDown("Left")) {
			SendPacking(_info.socket, PROTOCOL::C_DIR, LEFT);
			Log(LOG_STATE::SEND, "LeftButton");
		}
		if (INPUT->KeyDown("Down")) {
			SendPacking(_info.socket, PROTOCOL::C_DIR, DOWN);
			Log(LOG_STATE::SEND, "DownButton");
		}
		if (INPUT->KeyDown("Change")) {
			SendPacking(_info.socket, PROTOCOL::C_DIR, CHANGE);
			Log(LOG_STATE::SEND, "ChangeButton");
		}
		if (INPUT->KeyDown("Space")) {
			SendPacking(_info.socket, PROTOCOL::C_DIR, SPACE);
			_time = 0.0f;
			Log(LOG_STATE::SEND, "SpaceButton");
		}

		// �̰� �翬�� �ֽ��ϴ�.
		// �������ϳ����� �ð��� �־ �� �뿡 �ִ� �ð��� ��ȸ�ؼ� �ٿ��ų�� ������ �ߴµ�
		// ���� ���⸦ �ϴ� ������ ���� �÷��̸� �ϴ� ���̱� ������
		// ���� �°� ���ǵ带 �ο��ϴ� ���� �� ���ٰ� �����ؼ� Ŭ�󿡼� ���������߽��ϴ�.
		// �ð��� �Ϻ��� ���� �ý����� �ҷ��� DB�� �ƿ� ���Ҳ���� �����ؼ� �ϴ� �̷��� �߽��ϴ�.
		_time += DELTATIME;
		if (_maxTime > _time) {
			return;
		}
		_time -= _maxTime;

		SendPacking(_info.socket, PROTOCOL::C_BLOCK_DOWN);
		Log(LOG_STATE::SEND, "AutoDown");
	}
}

void ServerScene::GameState_Target()
{
	// ��� �������� ����ϴ�.
	// �÷��̾�� �ٸ��� ���� ��� ������ �н��߽��ϴ�.

	// ----------- ������ ----------
	{
		bool open = true;
		ImGui::Begin("##Board_Target", &open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
		ImGui::SetWindowPos(ImVec2{ 390.0f,0.0f });
		ImGui::SetWindowSize(ImVec2{ 250.0f, -1.0f });

		// Board�� ������ �̹��� ù ��° offset
		ImVec2 baseOffset = ImVec2{ 5.0f, 10.0f };
		for (int32 i = 0; i < Height; ++i) {
			for (int32 j = 0; j < Width; ++j) {
				ImGui::SetCursorPos(ImVec2{ (j * _blockSize) + baseOffset.x, (i * _blockSize) + baseOffset.y });
				if (_target->board[i][j] == 1) {
					ImGui::Image(_mapTexture[L"LockBlock"]->GetTexture(), ImVec2{ _blockSize, _blockSize });
				}
				else {
					ImGui::Image(_mapTexture[L"EmptyBlock"]->GetTexture(), ImVec2{ _blockSize, _blockSize });
				}
			}
		}

		// ----------- ������ ���� ������ ------------
		for (int32 i = 0; i < 4; ++i) {
			for (int32 j = 0; j < 4; ++j) {
				if (_target->block[i][j] == 1) {
					ImGui::SetCursorPos(ImVec2{ ((_target->posX + j) * _blockSize) + baseOffset.x, ((_target->posY + i) * _blockSize) + baseOffset.y });
					ImGui::Image(_mapTexture[L"Player"]->GetTexture(), ImVec2{ _blockSize, _blockSize });
				}
			}
		}

		ImGui::End();
	}

	// ----------- ������ ----------
	{
		bool open = true;
		ImGui::Begin("##NextBlock_Target", &open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
		ImGui::SetWindowPos(ImVec2{ 640.0f, 0.0f });
		ImGui::SetWindowSize(ImVec2{ 150.0f, 230.0f });

		ImGui::BeginChild("NextBlock", ImVec2{ 150.0f,100.0f });
		for (int32 i = 0; i < 4; ++i) {
			for (int32 j = 0; j < 4; ++j) {
				ImGui::SetCursorPos(ImVec2{ (j * 20.0f), (i * 20.0f) });
				if (_target->nextBlock[i][j] == 1) {
					ImGui::Image(_mapTexture[L"Player"]->GetTexture(), ImVec2{ 20.0f, 20.0f });
				}
				else {
					ImGui::Image(_mapTexture[L"EmptyBlock"]->GetTexture(), ImVec2{ 20.0f, 20.0f });
				}
			}
		}
		ImGui::EndChild();

		/* --------------- UserStat ---------------- */
		ImGui::BeginChild("##UserStat_Target", ImVec2{ 150.0f,80.0f });

		ImGui::Text("UserName : %s", &_target->name[0]);
		ImGui::Text("Score : %d", _target->score);

		ImGui::EndChild();

		ImGui::End();
	}
}

void ServerScene::RoomState()
{
	// �뿡 ������ ����Դϴ�.
	bool open = true;
	ImGui::Begin("RoomSystem", &open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	ImGui::SetWindowPos(ImVec2{ 100.0f, 40.0f });
	ImGui::SetWindowSize(ImVec2{ 600.0f, 400.0f });

	/* --------------- Room Input ---------------- */
	{
		// �����Ҷ� �ʿ��� ������ �˾Ƴ��ϴ�.
		ImGui::BeginChild("##InputPlayerName", ImVec2{ -1.0f, 80.0f }, true);
		{
			ImGui::BeginChild("##Input", ImVec2{ 400.0f, 60.0f }, true);
			ImGui::InputText("Name", &_player->name[0], 64);
			ImGui::InputText("Room", &_roomName[0], 64);
			ImGui::EndChild();
		}

		ImGui::SameLine();

		// ��ư�� ������ ������ ���� �˴ϴ�.
		if (ImGui::Button("In", ImVec2{-1.0f, -1.0f })) {
			SendPacking(_info.socket, PROTOCOL::C_START, &_roomName[0], &_player->name[0]);
			Log(LOG_STATE::SEND, "InGame");
		}

		ImGui::EndChild();
	}

	/* --------------- Room List ---------------- */
	{
		ImGui::BeginChild("##RoomList", ImVec2{ 300.0f,-1.0f }, true);

		// �뿡 ���� ������ �����ݴϴ�.
		int32 size = _roomList.size();
		for (int32 i = 0; i < size; ++i) {
			// ��ư ������ �������ݴϴ�.
			if (_roomList[i].count != 2) {
				// ���Ӱ���
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.3f, 0.76f, 0.0f, 1.0f });
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.96f, 0.5f, 1.0f });
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.5f, 0.9f, 0.5f, 1.0f });
			}
			else {
				// ���� �Ұ���
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.76f, 0.1f, 0.0f, 1.0f });
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.96f, 0.1f, 0.3f, 1.0f });
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.96f, 0.96f, 0.5f, 1.0f });
			}

			// �뿡 ���� ������ ����մϴ�.
			string roomName = _roomList[i].name + "##" + to_string(i);
			if (ImGui::Button(&roomName[0], ImVec2{ 200.0f, 20.0f })) {
				_roomName = _roomList[i].name;
			}
			ImGui::SameLine();
			
			string tex = to_string(_roomList[i].count) + "/ 2";
			ImGui::Text(&tex[0]);

			ImGui::PopStyleColor(3);
		}

		ImGui::EndChild();
	}

	ImGui::SameLine();

	/* ------------ ���� �߰��ϴ� ���Դϴ�. ------------ */
	{
		ImGui::BeginChild("##AddRoom", ImVec2{ -1.0f,-1.0f }, true);

		// ���� ����� ���� ������ �Է��մϴ�.
		static char roomBuffer[64] = { 0 };
		ImGui::Text("Room Add");
		ImGui::InputText("Room Name", roomBuffer, 64);
		if (ImGui::Button("Room Create")) {
			// ������ư�� ������ �����˴ϴ�.
			SendPacking(_info.socket, PROTOCOL::C_ROOM_CREATE, roomBuffer);
		}

		ImGui::EndChild();
	}

	ImGui::End();
}

void ServerScene::ResultState()
{
	// ������ ������ ��� ����� �����ݴϴ�.
	ImGui::Begin("##ResultState");
	ImGui::SetWindowSize("##ResultState", ImVec2{ (float)GET_SINGLE(Engine)->GetInfo().width , (float)GET_SINGLE(Engine)->GetInfo().height });
	ImGui::SetWindowPos(ImVec2{ 0.0f,0.0f });

	if (_state == STATE::WIN) {
		ImGui::Image(_mapTexture[L"Win"]->GetTexture(), ImVec2{ GET_SINGLE(Engine)->GetInfo().width / 2.0f, (float)GET_SINGLE(Engine)->GetInfo().height });
	}
	else {
		ImGui::Image(_mapTexture[L"Faild"]->GetTexture(), ImVec2{ GET_SINGLE(Engine)->GetInfo().width / 2.0f, (float)GET_SINGLE(Engine)->GetInfo().height });
	}

	ImGui::SameLine();

	{
		ImGui::BeginChild("##ScoreBoard");
		{ 
			// ���� �÷��̾��� ����� �����ݴϴ�.
			ImGui::BeginChild("Player##Score", ImVec2{ -1.0f, (float)GET_SINGLE(Engine)->GetInfo().height / 2.0f });

			ImGui::Text("Name : %s", &_player->name[0]);
			ImGui::Text("Score : %d", _player->score);

			ImGui::EndChild();
		}

		{
			// ���� �÷��̾��� ����� �����ݴϴ�.
			ImGui::BeginChild("Target##Score", ImVec2{ -1.0f, (float)GET_SINGLE(Engine)->GetInfo().height / 2.0f });

			ImGui::Text("Name : %s", &_target->name[0]);
			ImGui::Text("Score : %d", _target->score);

			ImGui::EndChild();
		}
		ImGui::EndChild();
	}

	_resultTime += DELTATIME;
	if (_resultTime > _resultMaxTime) {
		_resultTime = 0.0f;
		SendPacking(_info.socket, PROTOCOL::C_ROOM);
	}

	ImGui::End();
}

void ServerScene::ProcessStart()
{
	// ������Ʈ�� ���¸� �����մϴ�.
	_state = STATE::GAME;
	_isWait = false;

	_player->score = 0;
	_target->score = 0;

	Log(LOG_STATE::RECV, "ProcessStart");
}

void ServerScene::ProcessExit()
{
	// ������ �����մϴ�.
	_isRecvProcess = false;

	Log(LOG_STATE::RECV, "ProcessExit");
}

void ServerScene::ProcessBoardInfo()
{
	int32 id = 0;
	// �����ǿ� ���� ������ �����ɴϴ�.
	UnPacking(&_info.recvPtr, &id);

	BoardInfo* ptr = nullptr;
	if (id == 0) {
		ptr = _player;
	}
	else {
		ptr = _target;
	}

	int32 score = ptr->score;
	ptr->name.resize(64);
	UnPacking(&_info.recvPtr, &ptr->score, &ptr->name[0]);

	if (score != ptr->score && id == 0) {
		// ������ �޶����� ȿ����
		GET_SINGLE(SoundManager)->Play("Score");
	}

	// ������ ������ ����
	for (int32 i = 0; i < Height; ++i) {
		for (int32 j = 0; j < Width; ++j) {
			UnPacking(&_info.recvPtr, ptr->board[i][j]);
		}
	}

	// ���� �÷��̾��� ��ġ�� �˾ƿɴϴ�.
	UnPacking(&_info.recvPtr, &ptr->posX, &ptr->posY);

	// ���� �÷��̾��� ��� ��縦 �˾ƿɴϴ�
	for (int32 i = 0; i < 4; ++i) {
		for (int32 j = 0; j < 4; ++j) {
			UnPacking(&_info.recvPtr, ptr->block[i][j]);
		}
	}

	// ���� �÷��̾��� ���� ��� ��縦 �˾ƿɴϴ�
	for (int32 i = 0; i < 4; ++i) {
		for (int32 j = 0; j < 4; ++j) {
			UnPacking(&_info.recvPtr, ptr->nextBlock[i][j]);
		}
	}

	Log(LOG_STATE::RECV, "ProcessBoardInfo");
}

void ServerScene::ProcessRoom()
{
	_player->score = 0;
	_target->score = 0;

	int32 size = 0;
	UnPacking(&_info.recvPtr, &size);		// �� ����Ʈ�� ���̸� �޽��ϴ�.

	// �뿡 ���� ������ �޽��ϴ�.
	int32 count = 0;
	_roomList.clear();
	for (int32 i = 0; i < size; ++i) {
		char buffer[512] = { 0 };
		UnPacking(&_info.recvPtr, buffer, &count);				// ���� ������
		_roomList.push_back(RoomInfo{ buffer, count });			// ���� �̸�
	}

	if (_state == STATE::ROOM) {
		MessageBox(nullptr, L"�� ������ �ȵ˴ϴ�.", L"Room", MB_OK);
	}

	// ���� ���¸� �����մϴ�.
	_state = STATE::ROOM;

	Log(LOG_STATE::RECV, "ProcessRoom");
}

void ServerScene::ProcessRoomUpadte()
{
	int32 size = 0;
	UnPacking(&_info.recvPtr, &size);		// �� ����Ʈ�� ���̸� �޽��ϴ�.

	// �뿡 ������ �޽��ϴ�.
	int32 count = 0;
	_roomList.clear();
	for (int32 i = 0; i < size; ++i) {
		char buffer[512] = { 0 };
		UnPacking(&_info.recvPtr, buffer, &count);			   // ���� ������
		_roomList.push_back(RoomInfo{ buffer, count });		   // ���� �̸�
	}

	// ���¸� ������ �����ϴ� �� ���� �ʴٰ� ����

	Log(LOG_STATE::RECV, "ProcessRoomUpadte");
}

void ServerScene::ProcessGameWait()
{
	// ��ٸ��� ���̶�� �ΰ��� ȭ������ ��ȯ�ϰ� ��ٸ��� �����.
	_isWait = true;
	_state = STATE::GAME;

	SendPacking(_info.socket, PROTOCOL::C_GAME_WAIT);

	// �ʱ�ȭ�۾�
	_player->board = vector<vector<char>>(Height, vector<char>(Width, 0));
	_player->block = vector<vector<char>>(4, vector<char>(4, 0));
	_player->nextBlock = vector<vector<char>>(4, vector<char>(4, 0));

	// �ʱ�ȭ�۾�
	_target->board = vector<vector<char>>(Height, vector<char>(Width, 0));
	_target->block = vector<vector<char>>(4, vector<char>(4, 0));
	_target->nextBlock = vector<vector<char>>(4, vector<char>(4, 0));

	_player->name.clear();
	_target->name.clear();

	Log(LOG_STATE::RECV, "ProcessGameWait");
}

void ServerScene::ProcessGameWin()
{
	// ���¸� ��ȯ�մϴ�.
	_state = STATE::WIN;

	Log(LOG_STATE::RECV, "ProcessGameWin");
}

void ServerScene::ProcessGameFaild()
{
	// ���¸� ��ȯ�մϴ�.
	_state = STATE::FAILED;

	Log(LOG_STATE::RECV, "ProcessGameFaild");
}

void ServerScene::LogPresent()
{
	// ����׿� ���Ǳ��
	//ImGui::Begin("Log Send");
	//for (auto& log : _log[LOG_STATE::SEND]) {
	//	string str = log.str + " " + to_string(log.count);
	//	ImGui::Text(str.data());
	//}
	//ImGui::End();

	//ImGui::Begin("Log Recv");
	//for (auto& log : _log[LOG_STATE::RECV]) {
	//	string str = log.str + " " + to_string(log.count);
	//	ImGui::Text(str.data());
	//}
	//ImGui::End();

	//ImGui::Begin("Log Game");
	//for (auto& log : _log[LOG_STATE::GAME]) {
	//	string str = log.str + " " + to_string(log.count);
	//	ImGui::Text(str.data());
	//}
	//ImGui::End();

}

void ServerScene::Log(LOG_STATE state, const string& str)
{
	LogInfo info{ str, 0 };

	bool check = false;
	for (auto& log : _log[state]) {
		if (log.str.compare(str) == 0) {
			log.count += 1;
			check = true;
		}
	}

	if (check == false) {
		_log[state].push_back(info);
	}
}
