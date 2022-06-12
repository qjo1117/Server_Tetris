#pragma once



const int Width = 8;
const int Height = 15;

class Board
{
public:
	~Board() { }
	void Init();

	int SendPack(SocketInfo* p_info, int id);

	// 0 y, ȸ�� �Ұ� / 1 x �Ұ� / 2 ����
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

	// Thread�� �׾�淡 ���ڼ��� ���ϰ� �ο�
	atomic<int> _blockShape = 0;
	atomic<int> _nextShape = 0;
	atomic<int> _changeCount = 0;

	atomic<int> _score = 0;

	static Lock _lock;
};

