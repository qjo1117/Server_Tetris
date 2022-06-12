#include "pch.h"
#include "Board.h"
#include "BlockShape.h"

Lock Board::_lock;

void Board::Init()
{
	_blockShape = rand() % 7;
	_nextShape = rand() % 7;
	_pos = { 0,0 };
}

int Board::SendPack(SocketInfo* p_info, int id)
{
	lock_guard<Lock> lockGuard(_lock);

	// Block Down과 Dir Key입력이 동시에 진행되기때문에 막아둔다.
	char* sendPtr = p_info->sendbuf + sizeof(int);
	// 세부 정보를 패킹
	int size = Packing(&sendPtr, PROTOCOL::S_GAME_BOARD_INFO, id, _score.load());

	// 보드 전체를 패킹
	for (int i = 0; i < Height; ++i) {
		for (int j = 0; j < Width; ++j) {
			size += Packing(&sendPtr, _arrBoard[i][j]);
		}
	}

	// 플레이어 정보를 패킹
	size += Packing(&sendPtr, (int)_pos.x, (int)_pos.y);

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			size += Packing(&sendPtr, BlockShape[_blockShape * 4 + _changeCount][i][j]);
		}
	}

	// 다음 블록의 대한 정보를 전달해준다.
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			size += Packing(&sendPtr, BlockShape[_nextShape * 4][i][j]);
		}
	}

	memcpy(p_info->sendbuf, &size, sizeof(int));
	size += sizeof(int);

	return size;
}

int Board::CheckBlock()
{
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			// 현재 Player블록의 위치를 검사해야하는 경우
			if (BlockShape[_blockShape * 4 + _changeCount][i][j] == 1) {
				// 배열의 범위가 넘어가면 false
				if (!COMPARE(0 <= , _pos.y + i, < Height)) {
					return 0;
				}
				if (!COMPARE(0 <= , _pos.x + j, < Width)) {
					return 2;
				}

				// 배열에 접근했지만 뭔가 존재하는 경우 false
				if (_arrBoard[_pos.y + i][_pos.x + j] == 1) {
					return 0;
				}
			}
		}
	}

	return 1;
}


// Spacebar때문에 리턴값 추가
bool Board::MovePos(int x, int y)
{
	{
		// 자동으로 내려가는 작업과 유저가 입력하는 경우
		// 겹칠 수 있기때문에 락을 걸어준다.
		lock_guard<Lock> lockGuard(_lock);
		_pos.x += x; _pos.y += y;
	}

	int ret = CheckBlock();
	if (ret == 0 || ret == 2) {
		{
			// 뒤로 되돌려준다.
			lock_guard<Lock> lockGuard(_lock);
			_pos.x -= x; _pos.y -= y;
		}

		// 결과가 블록이 아래로 겹쳤을때만 문제가 생긴다.
		if (ret == 0) {
			NextBlock();
		}

		return true;
	}

	return false;
}

void Board::SpaceBar()
{
	// 스페이바를 누르면 바로 내려간다.
	for (int i = 0; i < Height; ++i) {
		if (MovePos(0, 1)) {
			return;
		}
	}
}

void Board::ChangeBlock()
{
	// 4개를 회전시켜서 움직이기때문
	int back = _changeCount;
	_changeCount.store((_changeCount + 1) % 4);

	int ret = CheckBlock();
	if (ret == 0  || ret == 2) {
		_changeCount.store(back);
	}

}

void Board::NextBlock()
{	
	// 보드판에 데이터를 추가하는 작업을 진행합니다.
	{
		lock_guard<Lock> lockGuard(_lock);
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {

				// 범위가 조금 괴랄하다?
				if (COMPARE(0 <=, _pos.x + j, < Width) == false) {
					continue;
				}
				if (COMPARE(0 <=, _pos.y + i, < Height) == false) {
					continue;
				}

				if (BlockShape[_blockShape * 4 + _changeCount][i][j] == 1) {
					_arrBoard[_pos.y + i][_pos.x + j] = BlockShape[_blockShape * 4 + _changeCount][i][j];
				}
			}
		}
	}

	LineClear();

	// 이제 해당하는 블록의 제어권을 잃었으므로 새로운 블록을 만들어냅니다.
	CreateBlock();


}

void Board::LineClear()
{
	// 아... 이런 로직 들어가는거 까먹고 있었습니다.
	// 책 소스코드 얻어와서 배낄껄...
	for (int i = 0; i < Height; ++i) {
		int count = 0;
		for (int j = 0; j < Width; ++j) {
			if (_arrBoard[i][j] == 1) {
				count += 1;
			}
		}

		// 클리어 대상
		if (count > Width - 1) {
			// Write

			// 클리어 
			lock_guard<Lock> lockGuard(_lock);
			for (int j = 0; j < Width; ++j) {
				_arrBoard[i][j] = 0;
			}

			// 스코어 증가
			_score.fetch_add(1);

			// 위에 있는 녀석들 순회해서 아래로 끄집어 내려놓는 작업
			for (int up = i; up > 0; --up) {
				for (int j = 0; j < Width; ++j) {
					_arrBoard[up][j] = _arrBoard[up - 1][j];
					_arrBoard[up - 1][j] = 0;
				}
			}
		}
	}
}

void Board::CreateBlock()
{
	lock_guard<Lock> lockGuard(_lock);
	// 랜덤하게 각 위치를 선정
	int select = rand() % 3;

	// 위치 지정
	switch (select) {
	case 0:
		_pos.x = 0; _pos.y = 0;
		break;
	case 1:
		_pos.x = 2; _pos.y = 0;
		break;
	case 2:
		_pos.x = 4; _pos.y = 0;
		break;
	}
	_blockShape.store(_nextShape);

	if (CheckBlock() == 0) {
		// 만약 생성이 가능하다면
		_changeCount.store(0);
	}
	else {
		// 안되면 게임오버

	}

	// 랜덤하게 다음 블럭을 지정해준다.
	_nextShape.store(rand() % 7);
}
