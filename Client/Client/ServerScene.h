#pragma once
#include "Scene.h"
#include "ClientInfo.h"


enum class STATE
{
    MENU,
    ROOM,
    GAME,
    END,
};

enum class LOG_STATE
{
    SEND,
    RECV,
    GAME,
};

class LogInfo
{
public:
    string str = "";
    int32 count = 0;
};

struct BoardInfo
{
    int32 posX = 0;
    int32 posY = 0;

    vector<vector<char>> board;
    vector<vector<char>> block;
    vector<vector<char>> nextBlock;

    int32 score = 0;
    string name = "UNKOWN";
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
    void LoadTexture();
    void RecvProcess();

    void InitProtocol();

    void Recv();

private:
    /* ----------- IMGUI --------- */
    // 원래는 Lib로 따로 뺐던것이였으나 서버 전용으로 만들기 위해
    // 급하게 수정했습니다.
    void ImGui_Init();
    void ImGui_Update();
    void ImGui_Render();
    void ImGui_End();

private:
    /* ----------- State --------- */
    void MenuState();
    void GameState_Player();
    void GameState_Target();
    void RoomState();

private:
    /* ----------- Recv Protocol --------- */
    void ProcessStart();
    void ProcessExit();
    void ProcessBoardInfo();
    void ProcessRoom();
    void ProcessGameWait();
    

private:
    void LogPresent();
    void Log(LOG_STATE state, const string& str);

private:
    SocketInfo _info = {};

    unordered_map<PROTOCOL, function<void()>> _mapProcess;

    thread _recvThread;
    bool _isRecvProcess = true;

    STATE _state = STATE::MENU;

    const int32 Width = 8;
    const int32 Height = 15;
    float _blockSize = 30.0f;

    // Player
    class BoardInfo* _player = nullptr;
    class BoardInfo* _target = nullptr;

    // Room
    vector<string> _roomList;
    string _roomName = "";
    bool _isWait = false;

    
    // Block or Board
    map<wstring, Ref<class Texture>> _mapTexture;

    float _time = 0.0f;
    float _maxTime = 3.0f;

    unordered_map<LOG_STATE, vector<LogInfo>> _log;
};

