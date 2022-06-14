#pragma once
#include "GameObject.h"


class Triangle :
    public GameObject
{
public:
    virtual void Init() override;
    virtual void End() override;
};

