#pragma once

#include "GameObject.h"

/* ----------------------
		Sphere
---------------------- */

struct SphereInfo
{
	float radius = 1.0f;
	uint32 stack = 15;
	uint32 slice = 15;
};


class Sphere : public GameObject
{
public:
	Sphere();
	virtual ~Sphere();
public:
	virtual void Init() override;
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void End() override;


protected:
	SphereInfo m_info = {};
};

