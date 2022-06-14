#pragma once

#include "GameObject.h"

struct TorusInfo
{
	float	innerRadius = 0.5f;
	float	outerRadius = 1.0f;
	uint32	sides = 20;
	uint32	rings = 20;
};

class Torus : public GameObject
{
public:
	virtual void Init();
	virtual void Awake();
	virtual void Update();
	virtual void End();

	TorusInfo& GetTorusInfo() { return m_info; }

private:
	TorusInfo m_info = {};
};

