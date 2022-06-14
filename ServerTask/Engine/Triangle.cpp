#include "pch.h"
#include "Triangle.h"

void Triangle::Init()
{
	GameObject::Init();
	vector<Vertex> vertex(3);

	vertex[0] = Vertex{ Vec3{0.0f,1.0f,0.0f}, Utils::Forward(),Vec2{0.5f,0.0f} };
	vertex[1] = Vertex{ Vec3{-1.0f,-1.0f,0.0f}, Utils::Forward(),Vec2{0.0f,1.0f} };
	vertex[2] = Vertex{ Vec3{1.0f,-1.0f,0.0f}, Utils::Forward(),Vec2{1.0f,1.0f} };

	vector<uint16> index(3);

	index[0] = 0; index[1] = 1; index[2] = 2;

	m_pMesh->Init(vertex, index);
}

void Triangle::End()
{
	GameObject::End();
}
