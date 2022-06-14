#pragma once
#include "Scene.h"
#include "ClientInfo.h"


// 클라이언트의 씬상태를 정의합니다.
enum class STATE
{
    MENU,
    ROOM,
    GAME,
    WIN,
    FAILED,
    END,
};

// 디버그용으로 만들었습니다.
enum class LOG_STATE
{
    SEND,
    RECV,
    GAME,
};

// 디버그용으로 만들었습니다.
class LogInfo
{
public:
    string str = "";            // 현재 문장
    int32 count = 0;            // 문장 갯수
};

// 보드판의 정보
struct BoardInfo
{
    // 블록의 현재 위치
    int32 posX = 0;
    int32 posY = 0;

    vector<vector<char>> board;         // 전체 보드판의 정보
    vector<vector<char>> block;         // 현재 플레이어의 정보
    vector<vector<char>> nextBlock;     // 다음 블록의 대한 정보

    int32 score = 0;                    // 현재 점수
    string name = "UNKOWN";             // 이름
};

// 룸에 대한 정보
struct RoomInfo
{
    string name = "";                   // 현재 룸 이름
    int32 count = 0;                    // 인원수
};

/*---------------
    ServerScene
----------------*/
class ServerScene :
    public Scene
{
public:
    /* ------------ 기본 함수 ------------ */
    virtual void Start() override;
    virtual void Update() override;
    virtual void Render() override;
    virtual void End() override;
    
private:
    /* ------------ 시작할때 쓰는 함수 ------------ */
    void ServerConnect();           // 윈속, 소켓, 연결
    void LoadTexture();             // 필요한 텍스처 맵핑
    void RecvProcess();             // Thread로 넘겨질 함수

    void InitProtocol();            // 가독성을 위한 함수

    void Recv();                    // 가독성을 위한 함수

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
    void MenuState();               // 첫번째 화면
    void GameState_Player();        // 인게임 자신의 영역
    void GameState_Target();        // 인게임 상대의 영역
    void RoomState();               // 룸에 대한 정보를 보여주는 화면
    void ResultState();             // 결과를 보여주는 화면

private:
    /* ----------- Recv Protocol --------- */
    void ProcessStart();            // 시작할때 필요한 정보를 얻거나 상태변환을 합니다. 
    void ProcessExit();             // 급해서 종료할 경우
    void ProcessBoardInfo();        // 보드판의 정보를 얻어옵니다.
    void ProcessRoom();             // 룸에 대한 정보를 얻어옵니다.
    void ProcessRoomUpadte();       // 현재 룸에 접속되어 있고 정보가 들어오면 갱신해줍니다.
    void ProcessGameWait();         // 상대가 들어올때까지 기다립니다.
    void ProcessGameWin();          // 이겼을때
    void ProcessGameFaild();        // 졌을때
    
private:
    // 람다식 때문에 넣었습니다.
    void EndRecvThread() {
        _isRecvProcess = false;
    }

private:
    /* ---------- Debug ---------- */
    void LogPresent();
    void Log(LOG_STATE state, const string& str);

private:
    // 소켓 정보
    SocketInfo _info = {};
    // 프로토콜, 실행할 함수
    unordered_map<PROTOCOL, function<void()>> _mapProcess;

    // Thread
    thread _recvThread;
    bool _isRecvProcess = true;

    // 현재 스테이트
    STATE _state = STATE::MENU;
    
    // 보드판의 기본 정보
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

    // 블록다운에 관련된 변수
    float _time = 0.0f;
    float _maxTime = 3.0f;

    // 디버그용
    unordered_map<LOG_STATE, vector<LogInfo>> _log;

    // 결과창 전용
    float _resultTime = 0.0f;
    float _resultMaxTime = 5.0f;
};

