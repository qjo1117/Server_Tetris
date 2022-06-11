#pragma once



#include "Scene.h"



class MeshScene :
    public Scene
{
public:
	/* ----- External Function ----- */
	virtual void Start() override;
	virtual void Update() override;
	virtual void End() override;

	void CameraMove();

private:
	Ref<class Tiger> m_pTiger = nullptr;
	Ref<class Camera> m_pCamera = nullptr;
};

