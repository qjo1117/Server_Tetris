#pragma once



const int Width = 8;
const int Height = 15;

class Board
{
public:
	~Board() { }
	void Init();

	int SendPack(SocketInfo* p_info, int id);

	// 0 y, 회전 불가 / 1 x 불가 / 2 가능
	int CheckBlock();

	bool MovePos(int x, int y);
	void SpaceBar();
	void ChangeBlock();

	void NextBlock();
	void LineClear();
	void CreateBlock();

private: 
	char _arrBoard[Height][Width] = { 0 };
	float _blockSize = 30.0f;

private:
	POINT _pos = { 0,0 };

	// Thread가 죽어가길래 원자성을 급하게 부여
	atomic<int> _blockShape = 0;
	atomic<int> _nextShape = 0;
	atomic<int> _changeCount = 0;

	atomic<int> _score = 0;

	static Lock _lock;
};

