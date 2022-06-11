#pragma once

#include "GameObject.h"

/* ----------------------
		Box
---------------------- */

struct BoxInfo
{
	float width = 1.0f;
	float height = 1.0f;
	float depth = 1.0f;
};

class Box : public GameObject
{
public:
	virtual ~Box();

public:
	virtual void Init() override;
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void End() override;

private:
	BoxInfo m_info;

};

