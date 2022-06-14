#pragma once
#include "Scene.h"
#include "ClientInfo.h"


// Ŭ���̾�Ʈ�� �����¸� �����մϴ�.
enum class STATE
{
    MENU,
    ROOM,
    GAME,
    WIN,
    FAILED,
    END,
};

// ����׿����� ��������ϴ�.
enum class LOG_STATE
{
    SEND,
    RECV,
    GAME,
};

// ����׿����� ��������ϴ�.
class LogInfo
{
public:
    string str = "";            // ���� ����
    int32 count = 0;            // ���� ����
};

// �������� ����
struct BoardInfo
{
    // ����� ���� ��ġ
    int32 posX = 0;
    int32 posY = 0;

    vector<vector<char>> board;         // ��ü �������� ����
    vector<vector<char>> block;         // ���� �÷��̾��� ����
    vector<vector<char>> nextBlock;     // ���� ����� ���� ����

    int32 score = 0;                    // ���� ����
    string name = "UNKOWN";             // �̸�
};

// �뿡 ���� ����
struct RoomInfo
{
    string name = "";                   // ���� �� �̸�
    int32 count = 0;                    // �ο���
};

/*---------------
    ServerScene
----------------*/
class ServerScene :
    public Scene
{
public:
    /* ------------ �⺻ �Լ� ------------ */
    virtual void Start() override;
    virtual void Update() override;
    virtual void Render() override;
    virtual void End() override;
    
private:
    /* ------------ �����Ҷ� ���� �Լ� ------------ */
    void ServerConnect();           // ����, ����, ����
    void LoadTexture();             // �ʿ��� �ؽ�ó ����
    void RecvProcess();             // Thread�� �Ѱ��� �Լ�

    void InitProtocol();            // �������� ���� �Լ�

    void Recv();                    // �������� ���� �Լ�

private:
    /* ----------- IMGUI --------- */
    // ������ Lib�� ���� �������̿����� ���� �������� ����� ����
    // ���ϰ� �����߽��ϴ�.
    void ImGui_Init();
    void ImGui_Update();
    void ImGui_Render();
    void ImGui_End();

private:
    /* ----------- State --------- */
    void MenuState();               // ù��° ȭ��
    void GameState_Player();        // �ΰ��� �ڽ��� ����
    void GameState_Target();        // �ΰ��� ����� ����
    void RoomState();               // �뿡 ���� ������ �����ִ� ȭ��
    void ResultState();             // ����� �����ִ� ȭ��

private:
    /* ----------- Recv Protocol --------- */
    void ProcessStart();            // �����Ҷ� �ʿ��� ������ ��ų� ���º�ȯ�� �մϴ�. 
    void ProcessExit();             // ���ؼ� ������ ���
    void ProcessBoardInfo();        // �������� ������ ���ɴϴ�.
    void ProcessRoom();             // �뿡 ���� ������ ���ɴϴ�.
    void ProcessRoomUpadte();       // ���� �뿡 ���ӵǾ� �ְ� ������ ������ �������ݴϴ�.
    void ProcessGameWait();         // ��밡 ���ö����� ��ٸ��ϴ�.
    void ProcessGameWin();          // �̰�����
    void ProcessGameFaild();        // ������
    
private:
    // ���ٽ� ������ �־����ϴ�.
    void EndRecvThread() {
        _isRecvProcess = false;
    }

private:
    /* ---------- Debug ---------- */
    void LogPresent();
    void Log(LOG_STATE state, const string& str);

private:
    // ���� ����
    SocketInfo _info = {};
    // ��������, ������ �Լ�
    unordered_map<PROTOCOL, function<void()>> _mapProcess;

    // Thread
    thread _recvThread;
    bool _isRecvProcess = true;

    // ���� ������Ʈ
    STATE _state = STATE::MENU;
    
    // �������� �⺻ ����
    const int32 Width = 8;
    const int32 Height = 15;
    float _blockSize = 30.0f;

    // Player
    class BoardInfo* _player;
    class BoardInfo* _target;

    // Room
    vector<RoomInfo> _roomList;
    string _roomName = "";
    bool _isWait = false;

    
    // Block or Board
    map<wstring, Ref<class Texture>> _mapTexture;

    // ��ϴٿ ���õ� ����
    float _time = 0.0f;
    float _maxTime = 3.0f;

    // ����׿�
    unordered_map<LOG_STATE, vector<LogInfo>> _log;

    // ���â ����
    float _resultTime = 0.0f;
    float _resultMaxTime = 5.0f;
};

