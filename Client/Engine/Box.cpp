#include "pch.h"
#include "Box.h"



Box::~Box()
{
}

void Box::Init()
{
	GameObject::Init();

	float w2 = m_info.width;
	float h2 = m_info.height;
	float d2 = m_info.depth;

	vector<Vertex> vertex(24);
	// 앞면
	vertex[0] = Vertex(Vec3(-w2, -h2, -d2), Vec3(0.0f, 0.0f, -1.0f), Vec2(0.0f, 1.0f));
	vertex[1] = Vertex(Vec3(-w2, +h2, -d2), Vec3(0.0f, 0.0f, -1.0f), Vec2(0.0f, 0.0f));
	vertex[2] = Vertex(Vec3(+w2, +h2, -d2), Vec3(0.0f, 0.0f, -1.0f), Vec2(1.0f, 0.0f));
	vertex[3] = Vertex(Vec3(+w2, -h2, -d2), Vec3(0.0f, 0.0f, -1.0f), Vec2(1.0f, 1.0f));
	// 뒷면
	vertex[4] = Vertex(Vec3(-w2, -h2, +d2), Vec3(0.0f, 0.0f, 1.0f), Vec2(1.0f, 1.0f));
	vertex[5] = Vertex(Vec3(+w2, -h2, +d2), Vec3(0.0f, 0.0f, 1.0f), Vec2(0.0f, 1.0f));
	vertex[6] = Vertex(Vec3(+w2, +h2, +d2), Vec3(0.0f, 0.0f, 1.0f), Vec2(0.0f, 0.0f));
	vertex[7] = Vertex(Vec3(-w2, +h2, +d2), Vec3(0.0f, 0.0f, 1.0f), Vec2(1.0f, 0.0f));
	// 윗면
	vertex[8] = Vertex(Vec3(-w2, +h2, -d2), Vec3(0.0f, 1.0f, 0.0f), Vec2(0.0f, 1.0f));
	vertex[9] = Vertex(Vec3(-w2, +h2, +d2), Vec3(0.0f, 1.0f, 0.0f), Vec2(0.0f, 0.0f));
	vertex[10] = Vertex(Vec3(+w2, +h2, +d2), Vec3(0.0f, 1.0f, 0.0f), Vec2(1.0f, 0.0f));
	vertex[11] = Vertex(Vec3(+w2, +h2, -d2), Vec3(0.0f, 1.0f, 0.0f), Vec2(1.0f, 1.0f));
	// 아랫면
	vertex[12] = Vertex(Vec3(-w2, -h2, -d2), Vec3(0.0f, -1.0f, 0.0f), Vec2(1.0f, 1.0f));
	vertex[13] = Vertex(Vec3(+w2, -h2, -d2), Vec3(0.0f, -1.0f, 0.0f), Vec2(0.0f, 1.0f));
	vertex[14] = Vertex(Vec3(+w2, -h2, +d2), Vec3(0.0f, -1.0f, 0.0f), Vec2(0.0f, 0.0f));
	vertex[15] = Vertex(Vec3(-w2, -h2, +d2), Vec3(0.0f, -1.0f, 0.0f), Vec2(1.0f, 0.0f));
	// 왼쪽면
	vertex[16] = Vertex(Vec3(-w2, -h2, +d2), Vec3(-1.0f, 0.0f, 0.0f), Vec2(0.0f, 1.0f));
	vertex[17] = Vertex(Vec3(-w2, +h2, +d2), Vec3(-1.0f, 0.0f, 0.0f), Vec2(0.0f, 0.0f));
	vertex[18] = Vertex(Vec3(-w2, +h2, -d2), Vec3(-1.0f, 0.0f, 0.0f), Vec2(1.0f, 0.0f));
	vertex[19] = Vertex(Vec3(-w2, -h2, -d2), Vec3(-1.0f, 0.0f, 0.0f), Vec2(1.0f, 1.0f));
	// 오른쪽면
	vertex[20] = Vertex(Vec3(+w2, -h2, -d2), Vec3(1.0f, 0.0f, 0.0f), Vec2(0.0f, 1.0f));
	vertex[21] = Vertex(Vec3(+w2, +h2, -d2), Vec3(1.0f, 0.0f, 0.0f), Vec2(0.0f, 0.0f));
	vertex[22] = Vertex(Vec3(+w2, +h2, +d2), Vec3(1.0f, 0.0f, 0.0f), Vec2(1.0f, 0.0f));
	vertex[23] = Vertex(Vec3(+w2, -h2, +d2), Vec3(1.0f, 0.0f, 0.0f), Vec2(1.0f, 1.0f));

	vector<uint16> index(36);

	// 앞면
	index[0] = 0; index[1] = 1; index[2] = 2;
	index[3] = 0; index[4] = 2; index[5] = 3;

	// 뒷면
	index[6] = 4; index[7] = 5; index[8] = 6;
	index[9] = 4; index[10] = 6; index[11] = 7;

	// 윗면
	index[12] = 8; index[13] = 9; index[14] = 10;
	index[15] = 8; index[16] = 10; index[17] = 11;

	// 아랫면
	index[18] = 12; index[19] = 13; index[20] = 14;
	index[21] = 12; index[22] = 14; index[23] = 15;

	// 왼쪽면
	index[24] = 16; index[25] = 17; index[26] = 18;
	index[27] = 16; index[28] = 18; index[29] = 19;

	// 오른쪽면
	index[30] = 20; index[31] = 21; index[32] = 22;
	index[33] = 20; index[34] = 22; index[35] = 23;

	/* ------ Normal 값때문에 각각 설정해준다 ------ */
	m_pMesh->Init(vertex, index);
}

void Box::Awake()
{
	GameObject::Awake();

}

void Box::Start()
{
	GameObject::Start();
}

void Box::Update()
{
	GameObject::Update();
}

void Box::End()
{
	GameObject::End();
}
