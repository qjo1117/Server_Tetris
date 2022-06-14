#include "pch.h"
#include "Camera.h"

Camera::Camera() 
{
    m_eType = GameObject_TYPE::Camera;
}

Camera::~Camera()
{
}

void Camera::Awake()
{
    GameObject::Awake();
    INPUT->AddKey("MouseBack", 'Z');
    INPUT->AddKey("MouseForward", 'C');
}

void Camera::Update()
{
    GameObject::Update();
    //{
    //    Vec3 move = Utils::Zero();
    //    float speed = 100.0f;
    //    if (INPUT->KeyPress("MouseForward")) {
    //        move.z += speed;
    //    }
    //    if (INPUT->KeyPress("MouseBack")) {
    //        move.z -= speed;
    //    }

    //    move *= DELTATIME;

    //    GetTransform()->Translation(move);
    //}
}

void Camera::FinalUpdate()
{
    GameObject::FinalUpdate();
    

    ViewMatrixUpdate();
    ProjMatrixUpdate();
}

void Camera::ViewMatrixUpdate()
{
    Vec3 eye = GetTransform()->GetWorldPosition();
    Vec3 look = eye;
    D3DXMatrixLookAtLH(&m_matView, &eye, &m_info.look, &m_info.viewUp);
    DEVICE->SetTransform(D3DTS_VIEW, &m_matView);
}

void Camera::ProjMatrixUpdate()
{
    D3DXMatrixPerspectiveFovLH(&m_matProj, m_info.fov, m_info.aspect, m_info.Near, m_info.Far);
    DEVICE->SetTransform(D3DTS_PROJECTION, &m_matProj);
}
