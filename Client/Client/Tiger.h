#pragma once
#include "GameObject.h"
class Tiger :
    public GameObject
{
public:
	virtual void Init() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void End() override;

private:

};

