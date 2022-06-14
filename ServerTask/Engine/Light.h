#pragma once



#include "GameObject.h"


/* ----------------------
		Light
---------------------- */

// 캐스팅을 이용해서 값을 전달하자.
struct LightInfo
{
    D3DLIGHTTYPE    type = D3DLIGHTTYPE::D3DLIGHT_DIRECTIONAL;            
    D3DCOLORVALUE   diffuse = Utils::White();
    D3DCOLORVALUE   specular = Utils::White();
    D3DCOLORVALUE   ambient = Utils::White();
    D3DVECTOR       position = Utils::One();
    D3DVECTOR       direction = Utils::Down();
    float           range = 1000.0f;           
    float           falloff = 1.0f;         
    float           attenuation0 = 0.0f;    
    float           attenuation1 = 0.125f;
    float           attenuation2 = 0.0f;    
    float           theta = 0.0f;           
    float           phi = 0.0f;             
};

class Light : public GameObject
{
public:
    Light();
    virtual ~Light();
public:
	virtual void Awake();
	virtual void FinalUpdate();

    void SetLightUpdate();


    LightInfo& GetLightInfo() { return m_light; }
private:
	LightInfo m_light = {};
    uint32 m_iIndex = 0;

    static uint32 LightCount;
};

