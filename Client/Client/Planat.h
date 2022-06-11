#pragma once

#include "Sphere.h"

class Planat :
    public Sphere
{
public:
	Planat();
	virtual ~Planat();
public:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void End() override;

	void SetSpeed(float p_fSpeed);

private:
	float m_fSpeed = 1.0f;
};

