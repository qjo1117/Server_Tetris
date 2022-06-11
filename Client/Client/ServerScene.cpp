#include "pch.h"
#include "ServerScene.h"
#include "Engine.h"

#include "Core.h"

void ServerScene::Start()
{
	Scene::Start();

	// IMGUI Init
	ImGui_Init();

	// 연결과 Recv를 받는 쓰레드를 생성한다.
	ServerConnect();
	


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
	thread recvThread([self = this]() { self->RecvProcess(); });
	_recvThread.swap(recvThread);
}

void ServerScene::RecvProcess()
{
	int retVal = 0;
	// 시작

	string str = "Hello";


	while (_isRecvProcess) {
		Recv();

		PROTOCOL protocol = PROTOCOL::NONE;

		// Unpacket Protocol
		
		if (_mapProcess[protocol]) {
			_mapProcess[protocol]();
		}


	}
}

// Recv부분이 길어서 따로 뺐습니다.
void ServerScene::Recv()
{
	int retVal = 0;
	int size = MAX_PATH;
	/*retVal = recv(_info.socket, (char*)&size, sizeof(int), 0);
	if (retVal == SOCKET_ERROR || retVal == 0) {
		MessageBox(nullptr, L"RecvSize", L"Error", MB_OK);
		_isRecvProcess = false;
		Core::loop = false;
		return;
	}*/

	retVal = recv(_info.socket, _info.recvBuffer, size, 0);
	if (retVal == SOCKET_ERROR || retVal == 0) {
		MessageBox(nullptr, L"Recv", L"Error", MB_OK);
		_isRecvProcess = false;
		Core::loop = false;
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
	//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();


	ImGui_ImplWin32_Init(GET_SINGLE(Engine)->GetInfo().hWnd);
	ImGui_ImplDX9_Init(DEVICE);
}

void ServerScene::ImGui_Update()
{
	//  Start the Dear ImGui frame
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();


	ImGui::NewFrame();

	switch (_state) {
	case STATE::MENU:
		MenuState();
		break;

	case STATE::GAME:
		GameState();
		break;

	default:
		// TODO : 종료
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

	ImGui::Image(0, ImVec2{ 500.0f,300.0f });

	// 메뉴바 Line
	ImGui::SetCursorPosX(200.0f);

	float buttonOffset = 14.0f;
	ImGui::BeginChild("##MenuChild", ImVec2(100.0f, 80.0f), true);
	ImGui::SetCursorPosX(buttonOffset);
	if (ImGui::Button(u8"Start", ImVec2(70.0f, 30.0f))) {
		// TODO : 이거 프로토콜 보내는 걸로 바꿔야함.
		_state = STATE::GAME;
	}
	ImGui::SetCursorPosX(buttonOffset);
	if (ImGui::Button(u8"Exit", ImVec2(70.0f, 30.0f))) {
		// TODO : 이거 프로토콜 보내는 걸로 바꿔야함.
		_state = STATE::END;
	}
	ImGui::EndChild();

	ImGui::End();
}

void ServerScene::GameState()
{
	{
		ImGui::Begin("##Board");

		float width = 20.0f;
		float height = 20.0f;
		
		// Board에 찍히는 이미지 첫 번째 offset
		ImVec2 baseOffset = ImVec2{ 10.0f, 30.0f };

		for (int32 i = 0; i < 10; ++i) {
			for (int32 j = 0; j < 17; ++j) {
				ImGui::SetCursorPos(ImVec2{ (i * width) + baseOffset.x, (j * height) + baseOffset.y });
				ImGui::Image(0, ImVec2{ width, height }); 
			}
		}

		ImGui::End();
	}

	{
		ImGui::Begin("##NextBlock");

		ImGui::BeginChild("NextBlock", ImVec2{ 80.0f,80.0f });

		ImGui::EndChild();


		ImGui::BeginChild("UserStat", ImVec2{ 80.0f,80.0f });


		ImGui::Text("UserName : %s", u8"박정욱");
		ImGui::Text("Score : %d", 10);

		ImGui::EndChild();

		ImGui::End();
	}
}
