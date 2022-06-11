#pragma once

#include "Sphere.h"

/*----------
	Planet
-----------*/
class Planet :
    public Sphere
{
public:
	Planet();
	virtual ~Planet();
public:
	virtual void Init() override;
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void End() override;

	void SetSpeed(float p_fSpeed);

public:
	void SetPosition(const Vec3& pos) { m_position = pos; }
	Vec3 GetPosition() { return m_position; }

private:
	float m_fSpeed = 1.0f;
	Vec3 m_position = Utils::Zero();

	static bool m_bRotation;
};

