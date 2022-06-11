#pragma once


#include "Scene.h"

/*-------------------
	TextureScene
--------------------*/
class TextureScene :
    public Scene
{
public:
	

	virtual void Start();
	virtual void Update();
	virtual void End();

	void CameraMove();

private:
	Ref<Camera> m_pCamera = nullptr;
};

