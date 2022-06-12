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

	// Block Down�� Dir Key�Է��� ���ÿ� ����Ǳ⶧���� ���Ƶд�.
	char* sendPtr = p_info->sendbuf + sizeof(int);
	// ���� ������ ��ŷ
	int size = Packing(&sendPtr, PROTOCOL::S_GAME_BOARD_INFO, id, _score.load());

	// ���� ��ü�� ��ŷ
	for (int i = 0; i < Height; ++i) {
		for (int j = 0; j < Width; ++j) {
			size += Packing(&sendPtr, _arrBoard[i][j]);
		}
	}

	// �÷��̾� ������ ��ŷ
	size += Packing(&sendPtr, (int)_pos.x, (int)_pos.y);

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			size += Packing(&sendPtr, BlockShape[_blockShape * 4 + _changeCount][i][j]);
		}
	}

	// ���� ����� ���� ������ �������ش�.
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
			// ���� Player����� ��ġ�� �˻��ؾ��ϴ� ���
			if (BlockShape[_blockShape * 4 + _changeCount][i][j] == 1) {
				// �迭�� ������ �Ѿ�� false
				if (!COMPARE(0 <= , _pos.y + i, < Height)) {
					return 0;
				}
				if (!COMPARE(0 <= , _pos.x + j, < Width)) {
					return 2;
				}

				// �迭�� ���������� ���� �����ϴ� ��� false
				if (_arrBoard[_pos.y + i][_pos.x + j] == 1) {
					return 0;
				}
			}
		}
	}

	return 1;
}


// Spacebar������ ���ϰ� �߰�
bool Board::MovePos(int x, int y)
{
	{
		// �ڵ����� �������� �۾��� ������ �Է��ϴ� ���
		// ��ĥ �� �ֱ⶧���� ���� �ɾ��ش�.
		lock_guard<Lock> lockGuard(_lock);
		_pos.x += x; _pos.y += y;
	}

	int ret = CheckBlock();
	if (ret == 0 || ret == 2) {
		{
			// �ڷ� �ǵ����ش�.
			lock_guard<Lock> lockGuard(_lock);
			_pos.x -= x; _pos.y -= y;
		}

		// ����� ����� �Ʒ��� ���������� ������ �����.
		if (ret == 0) {
			NextBlock();
		}

		return true;
	}

	return false;
}

void Board::SpaceBar()
{
	// �����̹ٸ� ������ �ٷ� ��������.
	for (int i = 0; i < Height; ++i) {
		if (MovePos(0, 1)) {
			return;
		}
	}
}

void Board::ChangeBlock()
{
	// 4���� ȸ�����Ѽ� �����̱⶧��
	int back = _changeCount;
	_changeCount.store((_changeCount + 1) % 4);

	int ret = CheckBlock();
	if (ret == 0  || ret == 2) {
		_changeCount.store(back);
	}

}

void Board::NextBlock()
{	
	// �����ǿ� �����͸� �߰��ϴ� �۾��� �����մϴ�.
	{
		lock_guard<Lock> lockGuard(_lock);
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {

				// ������ ���� �����ϴ�?
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

	// ���� �ش��ϴ� ����� ������� �Ҿ����Ƿ� ���ο� ����� �������ϴ�.
	CreateBlock();


}

void Board::LineClear()
{
	// ��... �̷� ���� ���°� ��԰� �־����ϴ�.
	// å �ҽ��ڵ� ���ͼ� �賥��...
	for (int i = 0; i < Height; ++i) {
		int count = 0;
		for (int j = 0; j < Width; ++j) {
			if (_arrBoard[i][j] == 1) {
				count += 1;
			}
		}

		// Ŭ���� ���
		if (count > Width - 1) {
			// Write

			// Ŭ���� 
			lock_guard<Lock> lockGuard(_lock);
			for (int j = 0; j < Width; ++j) {
				_arrBoard[i][j] = 0;
			}

			// ���ھ� ����
			_score.fetch_add(1);

			// ���� �ִ� �༮�� ��ȸ�ؼ� �Ʒ��� ������ �������� �۾�
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
	// �����ϰ� �� ��ġ�� ����
	int select = rand() % 3;

	// ��ġ ����
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
		// ���� ������ �����ϴٸ�
		_changeCount.store(0);
	}
	else {
		// �ȵǸ� ���ӿ���

	}

	// �����ϰ� ���� ���� �������ش�.
	_nextShape.store(rand() % 7);
}
