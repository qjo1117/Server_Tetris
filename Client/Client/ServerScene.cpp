#include "pch.h"
#include "ServerScene.h"
#include "Engine.h"

#include "Core.h"
#include "ResourcesManager.h"
#include "SoundManager.h"
#include "Texture.h"
#include "Timer.h"
#include "../Server/Server/BlockShape.h"

void ServerScene::Start()
{
	this_thread::sleep_for(1s);

	Scene::Start();

	// IMGUI Init
	ImGui_Init();


	InitProtocol();
	LoadTexture();

	// 연결과 Recv를 받는 쓰레드를 생성한다.
	ServerConnect();
	
	_player = new BoardInfo();
	_target = new BoardInfo();

	/* --------------- 초기화 단계 --------------- */
	_player->board = vector<vector<char>>(Height, vector<char>(Width, '\0'));
	_player->block = vector<vector<char>>(4, vector<char>(4, '\0'));
	_player->nextBlock = vector<vector<char>>(4, vector<char>(4, '\0'));

	_target->board = vector<vector<char>>(Height, vector<char>(Width, 0));
	_target->block = vector<vector<char>>(4, vector<char>(4, 0));
	_target->nextBlock = vector<vector<char>>(4, vector<char>(4, 0));

	_player->name.resize(64);
	_target->name.resize(64);

	_roomName.resize(64);

	GET_SINGLE(SoundManager)->LoadSound("Score", false, "Score.wav");

	Core::GetI()->DestroyWindowAddFunc([self = static_pointer_cast<ServerScene>(shared_from_this())]() { SendPacking(self->_info.socket, PROTOCOL::C_EXIT); });
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

	// 혹시나하는 쓰레드가 종료되지 않는 경우까지 체크
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

	// 윈속 초기화
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

	// Recv Thread 시작
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
}

void ServerScene::RecvProcess()
{
	int retVal = 0;
	// 시작

	while (_isRecvProcess) {
		Recv();

		// 버퍼 제대로 만들기에는 시작 촉박
		_info.recvPtr = _info.recvBuffer;
		_info.sendPtr = _info.sendBuffer;

		PROTOCOL protocol = PROTOCOL::NONE;

		UnPacking(&_info.recvPtr, &protocol);
		
		// 프로토콜에 해당하는 함수를 호출한다.
		if (_mapProcess[protocol]) {
			_mapProcess[protocol]();
		}
	}

	Core::loop = false;
}

void ServerScene::InitProtocol()
{
	/* ----------- 프로토콜에 따른 함수 실행 ----------- */

	// 어차피 생존주기를 따지는 함수들이 아니다.
	// Scene이 존재하지않으면 종료된다.
	_mapProcess[PROTOCOL::S_START] = [self = this]() { self->ProcessStart(); };
	_mapProcess[PROTOCOL::EXIT] = [self = this]() { self->ProcessExit(); };
	_mapProcess[PROTOCOL::S_GAME_BOARD_INFO] = [self = this]() { self->ProcessBoardInfo(); };
	_mapProcess[PROTOCOL::S_ROOM] = [self = this]() { self->ProcessRoom(); };
	_mapProcess[PROTOCOL::S_GAME_WAIT] = [self = this]() { self->ProcessGameWait(); };

	// 생존주기를 걱정하면 이렇게 바꾸면 된다.
	//_mapProcess[PROTOCOL::S_START] = [self = shared_from_this()]() { static_pointer_cast<ServerScene>(self)->ProcessStart(); };
}

// Recv부분이 길어서 따로 뺐습니다.
void ServerScene::Recv()
{
	int retVal = 0;
	int size = MAX_PATH;
	retVal = recv(_info.socket, (char*)&size, sizeof(int), 0);
	if (retVal == SOCKET_ERROR || retVal == 0) {
		_isRecvProcess = false;
		Core::loop = false;
		MessageBox(nullptr, L"Size", L"Recv", MB_OK);
		return;
	}

	retVal = recv(_info.socket, _info.recvBuffer, size, 0);
	if (retVal == SOCKET_ERROR || retVal == 0) {
		_isRecvProcess = false;
		Core::loop = false;
		MessageBox(nullptr, L"Data", L"Recv", MB_OK);
		return;
	}
}

void ServerScene::ImGui_Init()
{
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
	//  Start the Dear ImGui frame
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();


	ImGui::NewFrame();

	LogPresent();

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

	default:
		// TODO : 종료
		SendPacking(_info.socket, PROTOCOL::C_EXIT);
		break;
	}

	ImGui::EndFrame();
}

void ServerScene::ImGui_Render()
{
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

void ServerScene::ImGui_End()
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void ServerScene::MenuState()
{
	bool open = true;
	ImGui::Begin("##Menu", &open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

	ImGui::Image(_mapTexture[L"Menu"]->GetTexture(), ImVec2{500.0f,300.0f});

	// 메뉴바 Line
	ImGui::SetCursorPosX(200.0f);

	// ----------- 메뉴 그리기 ------------
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

	ImGui::End();
}

void ServerScene::GameState_Player()
{

	// ----------- 게임판 ----------
	{
		ImGui::Begin("##Board_Player");
		
		// Board에 찍히는 이미지 첫 번째 offset
		ImVec2 baseOffset = ImVec2{ 10.0f, 30.0f };

		for (int32 i = 0; i < Height; ++i) {
			for (int32 j = 0; j < Width; ++j) {
				ImGui::SetCursorPos(ImVec2{ (j * _blockSize) + baseOffset.x, (i * _blockSize) + baseOffset.y });
				if (_player->board[i][j] == 1) {
					ImGui::Image(_mapTexture[L"LockBlock"]->GetTexture(), ImVec2{ _blockSize, _blockSize });
				}
				else {
					ImGui::Image(_mapTexture[L"EmptyBlock"]->GetTexture(), ImVec2{ _blockSize, _blockSize });
				}
			}
		}

		// ----------- 보드판 위에 덮어씌우기 ------------
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

	// ----------- 점수판 ----------
	{
		ImGui::Begin("##NextBlock_Player");

		ImGui::BeginChild("NextBlock", ImVec2{ 100.0f,100.0f });

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


		/* --------------- UserStat ---------------- */
		ImGui::BeginChild("##UserStat_Player", ImVec2{ 80.0f,80.0f });

		ImGui::Text("UserName : %s", &_player->name[0]);
		ImGui::Text("Score : %d", _player->score);
		ImGui::Text("pos : %d, %d", (int32)_player->posX, (int32)_player->posY);

		ImGui::EndChild();

		ImGui::End();
	}


	// Wait State
	if(_isWait == false) {
		// ----------- 입력에 따른 프로토콜 ------------
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
			Log(LOG_STATE::SEND, "SpaceButton");
		}

		// 타임이 지나면 아래로 내려간다는 패킷을 전달해준다.
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
	// ----------- 게임판 ----------
	{
		ImGui::Begin("##Board_Target");

		// Board에 찍히는 이미지 첫 번째 offset
		ImVec2 baseOffset = ImVec2{ 10.0f, 30.0f };

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

		// ----------- 보드판 위에 덮어씌우기 ------------
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

	// ----------- 점수판 ----------
	{
		ImGui::Begin("##NextBlock_Target");

		ImGui::BeginChild("NextBlock", ImVec2{ 100.0f,100.0f });
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
		ImGui::BeginChild("##UserStat_Target", ImVec2{ 80.0f,80.0f });

		ImGui::Text("UserName : %s", &_target->name[0]);
		ImGui::Text("Score : %d", _target->score);
		ImGui::Text("pos : %d, %d", (int32)_target->posX, (int32)_target->posY);

		ImGui::EndChild();

		ImGui::End();
	}
}

void ServerScene::RoomState()
{
	ImGui::Begin("RoomSystem");

	// 입력 구간
	{
		ImGui::BeginChild("##InputPlayerName", ImVec2{ -1.0f, 60.0f }, true);

		ImGui::InputText("Name", &_player->name[0], 64);
		ImGui::InputText("Room", &_roomName[0], 64);

		ImGui::SetCursorPos(ImVec2{ 325.0f, 5.0f });
		if (ImGui::Button("In", ImVec2{50.0f, 50.0f})) {
			SendPacking(_info.socket, PROTOCOL::C_START, &_roomName[0]);
			Log(LOG_STATE::SEND, "InGame");
		}

		ImGui::EndChild();
	}

	// 룸 리스트 구간
	{
		ImGui::BeginChild("##RoomList", ImVec2{ -1.0f,-1.0f }, true);

		int32 size = _roomList.size();

		for (int32 i = 0; i < size; ++i) {
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.3f, 0.76f, 0.0f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.96f, 0.5f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.5f, 0.9f, 0.5f, 1.0f });

			string roomName = _roomList[i] + "##" + to_string(i);
			if (ImGui::Button(&roomName[0], ImVec2{ 250.0f, 20.0f })) {
				_roomName = _roomList[i];
			}

			ImGui::PopStyleColor(3);
		}

		ImGui::EndChild();
	}

	ImGui::End();
}

void ServerScene::ProcessStart()
{
	_state = STATE::GAME;
	_isWait = false;

	Log(LOG_STATE::RECV, "ProcessStart");
}

void ServerScene::ProcessExit()
{
	_isRecvProcess = false;

	Log(LOG_STATE::RECV, "ProcessExit");
}

void ServerScene::ProcessBoardInfo()
{
	int32 id = 0;
	// 보드판에 대한 정보를 가져옵니다.
	UnPacking(&_info.recvPtr, &id);

	BoardInfo* ptr = nullptr;
	if (id == 0) {
		ptr = _player;
	}
	else {
		ptr = _target;
	}

	int32 score = ptr->score;
	UnPacking(&_info.recvPtr, &ptr->score);

	if (score != ptr->score && id == 0) {
		// 점수가 달라지면 효과음
		GET_SINGLE(SoundManager)->Play("Score");
	}

	// 보드의 정보를 셋팅
	for (int32 i = 0; i < Height; ++i) {
		for (int32 j = 0; j < Width; ++j) {
			UnPacking(&_info.recvPtr, ptr->board[i][j]);
		}
	}

	// 현재 플레이어의 위치를 알아옵니다.
	UnPacking(&_info.recvPtr, &ptr->posX, &ptr->posY);

	// 현재 플레이어의 블록 모양를 알아옵니다
	for (int32 i = 0; i < 4; ++i) {
		for (int32 j = 0; j < 4; ++j) {
			UnPacking(&_info.recvPtr, ptr->block[i][j]);
		}
	}

	// 현재 플레이어의 다음 블록 모양를 알아옵니다
	for (int32 i = 0; i < 4; ++i) {
		for (int32 j = 0; j < 4; ++j) {
			UnPacking(&_info.recvPtr, ptr->nextBlock[i][j]);
		}
	}

	Log(LOG_STATE::RECV, "ProcessBoardInfo");
}

void ServerScene::ProcessRoom()
{
	int32 size = 0;
	UnPacking(&_info.recvPtr, &size);

	char buffer[512] = { 0 };
	_roomList.clear();
	for (int32 i = 0; i < size; ++i) {
		UnPacking(&_info.recvPtr, buffer);
		_roomList.push_back(buffer);
	}

	if (_state == STATE::ROOM) {
		// TODO : 접속 안됬다고 전달해줘야함
		MessageBox(nullptr, L"룸 접속이 안됩니다.", L"Room", MB_OK);
	}
	_state = STATE::ROOM;

	Log(LOG_STATE::RECV, "ProcessRoom");
}

void ServerScene::ProcessGameWait()
{
	_isWait = true;
	_state = STATE::GAME;

	SendPacking(_info.socket, PROTOCOL::C_GAME_WAIT);

	Log(LOG_STATE::RECV, "ProcessGameWait");
}

void ServerScene::LogPresent()
{
	// 디버그용 편의기능
#ifdef _DEBUG

	ImGui::Begin("Log Send");
	for (auto& log : _log[LOG_STATE::SEND]) {
		string str = log.str + " " + to_string(log.count);
		ImGui::Text(str.data());
	}
	ImGui::End();

	ImGui::Begin("Log Recv");
	for (auto& log : _log[LOG_STATE::RECV]) {
		string str = log.str + " " + to_string(log.count);
		ImGui::Text(str.data());
	}
	ImGui::End();

	ImGui::Begin("Log Game");
	for (auto& log : _log[LOG_STATE::GAME]) {
		string str = log.str + " " + to_string(log.count);
		ImGui::Text(str.data());
	}
	ImGui::End();

#endif

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
