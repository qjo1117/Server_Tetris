#pragma once



#include "Box.h"



class TexCube :
    public Box
{
public:
	virtual void Init() override;
	virtual void Update() override;
	virtual void End() override;

private:

};

