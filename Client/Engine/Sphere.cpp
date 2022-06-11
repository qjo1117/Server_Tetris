#include "pch.h"
#include "Sphere.h"

Sphere::Sphere()
{

}

Sphere::~Sphere()
{
}

void Sphere::Init()
{
	GameObject::Init();

	float radius = m_info.radius; // ±∏¿« π›¡ˆ∏ß
	uint32 stackCount = m_info.stack; // ∞°∑Œ ∫–«“
	uint32 sliceCount = m_info.slice; // ºº∑Œ ∫–«“

	vector<Vertex> vertex;

	Vertex v;

	// ∫œ±ÿ
	v.pos = Vec3(0.0f, radius, 0.0f);
	v.uv = Vec2(0.5f, 0.0f);
	v.normal = Utils::Normalize(v.normal);
	vertex.push_back(v);

	float stackAngle = Utils::DX_PI / stackCount;
	float sliceAngle = Utils::DX_2PI / sliceCount;

	float deltaU = 1.f / static_cast<float>(sliceCount);
	float deltaV = 1.f / static_cast<float>(stackCount);

	// ∞Ì∏Æ∏∂¥Ÿ µπ∏Èº≠ ¡§¡°¿ª ∞ËªÍ«—¥Ÿ (∫œ±ÿ/≥≤±ÿ ¥‹¿œ¡°¿∫ ∞Ì∏Æ∞° X)
	for (uint32 y = 1; y <= stackCount - 1; ++y) {
		float phi = y * stackAngle;

		// ∞Ì∏Æø° ¿ßƒ°«— ¡§¡°
		for (uint32 x = 0; x <= sliceCount; ++x) {
			float theta = x * sliceAngle;

			v.pos.x = radius * sinf(phi) * cosf(theta);
			v.pos.y = radius * cosf(phi);
			v.pos.z = radius * sinf(phi) * sinf(theta);

			v.uv = Vec2(deltaU * x, deltaV * y);

			v.normal = v.pos;
			v.normal = Utils::Normalize(v.pos);

			vertex.push_back(v);
		}
	}

	// ≥≤±ÿ
	v.pos = Vec3(0.0f, -radius, 0.0f);
	v.uv = Vec2(0.5f, 1.0f);
	v.normal = Utils::Normalize(v.pos);
	vertex.push_back(v);

	vector<uint16> index(36);

	// ∫œ±ÿ ¿Œµ¶Ω∫
	for (uint32 i = 0; i <= sliceCount; ++i) {
		//  [0]
		//   |  \
		//  [i+1]-[i+2]
		index.push_back(0);
		index.push_back(i + 2);
		index.push_back(i + 1);
	}

	// ∏ˆ≈Î ¿Œµ¶Ω∫
	uint32 ringVertexCount = sliceCount + 1;
	for (uint32 y = 0; y < stackCount - 2; ++y) {
		for (uint32 x = 0; x < sliceCount; ++x) {
			//  [y, x]-[y, x+1]
			//  |		/
			//  [y+1, x]
			index.push_back(1 + (y)*ringVertexCount + (x));
			index.push_back(1 + (y)*ringVertexCount + (x + 1));
			index.push_back(1 + (y + 1) * ringVertexCount + (x));
			//		 [y, x+1]
			//		 /	  |
			//  [y+1, x]-[y+1, x+1]
			index.push_back(1 + (y + 1) * ringVertexCount + (x));
			index.push_back(1 + (y)*ringVertexCount + (x + 1));
			index.push_back(1 + (y + 1) * ringVertexCount + (x + 1));
		}
	}

	// ≥≤±ÿ ¿Œµ¶Ω∫
	uint32 bottomIndex = static_cast<uint32>(vertex.size()) - 1;
	uint32 lastRingStartIndex = bottomIndex - ringVertexCount;
	for (uint32 i = 0; i < sliceCount; ++i) {
		//  [last+i]-[last+i+1]
		//  |      /
		//  [bottom]
		index.push_back(bottomIndex);
		index.push_back(lastRingStartIndex + i);
		index.push_back(lastRingStartIndex + i + 1);
	}

	if (m_pMesh) {
		m_pMesh->Init(vertex, index);
	}
}

void Sphere::Awake()
{
	GameObject::Awake();

}

void Sphere::Start()
{
	GameObject::Start();
}

void Sphere::Update()
{
	GameObject::Update();
}

void Sphere::End()
{
	GameObject::End();
}
