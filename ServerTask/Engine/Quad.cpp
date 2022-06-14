#include "pch.h"
#include "Quad.h"

void Quad::Init()
{
	GameObject::Init();
	vector<Vertex> vertex(4);

	// 앞면
	vertex[0] = Vertex(Vec3(-1.0f, -1.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec2(0.0f, 1.0f));
	vertex[1] = Vertex(Vec3(-1.0f, +1.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec2(0.0f, 0.0f));
	vertex[2] = Vertex(Vec3(+1.0f, +1.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec2(1.0f, 0.0f));
	vertex[3] = Vertex(Vec3(+1.0f, -1.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec2(1.0f, 1.0f));

	vector<uint16> index(6);

	// 앞면
	index[0] = 0; index[1] = 1; index[2] = 2;
	index[3] = 0; index[4] = 2; index[5] = 3;

}

void Quad::End()
{
	GameObject::End();
}
