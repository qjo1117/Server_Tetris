#pragma once
#include "Scene.h"
#include "ClientInfo.h"


enum class STATE
{
    MENU,
    GAME,
    END,
};

/*---------------
    ServerScene
----------------*/
class ServerScene :
    public Scene
{
public:
    virtual void Start() override;
    virtual void Update() override;
    virtual void Render() override;
    virtual void End() override;
    
private:
    void ServerConnect();
    void RecvProcess();

    void Recv();

private:
    void ImGui_Init();
    void ImGui_Update();
    void ImGui_Render();
    void ImGui_End();

private:
    void MenuState();
    void GameState();

    

private:
    SocketInfo _info = {};

    unordered_map<PROTOCOL, function<void()>> _mapProcess;

    thread _recvThread;
    bool _isRecvProcess = true;

    STATE _state = STATE::MENU;
};

