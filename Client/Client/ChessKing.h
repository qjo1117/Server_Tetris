#pragma once

#include "GameObject.h"


class ChessKing :
    public GameObject
{
public:
	virtual void Init() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void End() override;
};

