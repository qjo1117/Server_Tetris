#include "pch.h"
#include "Mesh.h"
#include "Device.h"
#include "PathManager.h"


void Mesh::Init(vector<Vertex>& p_vecVertex, vector<uint16>& p_vecIndex)
{
	if (p_vecVertex.empty() == false) {
		SetVertexBuffer(p_vecVertex);

		if (p_vecIndex.empty() == false) {
			SetIndexBuffer(p_vecIndex);
		}
	}
}

void Mesh::Init(const wstring& fileName)
{
	LPD3DXBUFFER pBuffer = nullptr;

	wstring path = GET_SINGLE(PathManager)->FindPath(FBX_PATH_KEY) + fileName;
	D3DXLoadMeshFromX(path.c_str(), D3DXMESH_SYSTEMMEM, DEVICE, nullptr, 
		&pBuffer, nullptr, (DWORD*)&m_iNumMaterials, &m_pMesh);

	D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pBuffer->GetBufferPointer();

	m_pMaterials.resize(m_iNumMaterials);
	m_pTexMaterials.resize(m_iNumMaterials);

	for (uint64 i = 0; i < m_iNumMaterials; i++) { 
		m_pMaterials[i] = d3dxMaterials[i].MatD3D;
		m_pMaterials[i].Ambient = m_pMaterials[i].Diffuse;

		m_pTexMaterials[i] = NULL;
		if (d3dxMaterials[i].pTextureFilename != NULL &&
			lstrlenA(d3dxMaterials[i].pTextureFilename) > 0) {

			if (FAILED(D3DXCreateTextureFromFileA(DEVICE,
				d3dxMaterials[i].pTextureFilename,
				&m_pTexMaterials[i]))) {

				char buffer[BUFSIZ] = { 0 };
				::strcat(buffer, Utils::Wstr2Str(GET_SINGLE(PathManager)->FindPath(TEXTURE_PATH_KEY)).c_str());
				::strcat(buffer, d3dxMaterials[i].pTextureFilename);

				D3DXCreateTextureFromFileA(DEVICE, buffer, &m_pTexMaterials[i]);
			}
		}
	}

	// Done with the material buffer
	pBuffer->Release();
}

void Mesh::Render()
{
	if (m_pMesh) {
		for (uint64 i = 0; i < m_iNumMaterials; i++) {
			// Set the material and texture for this subset
			DEVICE->SetMaterial(&m_pMaterials[i]);
			DEVICE->SetTexture(0, m_pTexMaterials[i]);

			// Draw the mesh subset
			m_pMesh->DrawSubset(i);
		}
	} 
	else if (m_iVertexSize > 0) {
		DEVICE->SetStreamSource(0, m_pVertex, 0, sizeof(Vertex));
		DEVICE->SetFVF(D3DFVF_CUSTOMVERTEX);

		if (m_iIndexSize > 0) {
			DEVICE->SetIndices(m_pIndex);
		}

		DEVICE->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_iVertexSize, 0, m_iIndexSize / 3);
	}
}

void Mesh::End()
{
	if (m_pVertex) {
		m_pVertex->Release();
		m_pVertex = nullptr;
	}
	if (m_pIndex) {
		m_pIndex->Release();
		m_pIndex = nullptr;
	}

	if (m_pTexMaterials.empty() == false) {
		for (uint64 i = 0; i < m_iNumMaterials; ++i) {
			if (m_pTexMaterials[i])
				m_pTexMaterials[i]->Release();
			m_pTexMaterials[i] = nullptr;
		}
	}
}


void Mesh::SetVertexBuffer(vector<Vertex>& p_Vertex)
{
	m_iVertexSize = p_Vertex.size();
	int32 length = m_iVertexSize * sizeof(Vertex);
	DEVICE->CreateVertexBuffer(length, D3DUSAGE_WRITEONLY, D3DFVF_CUSTOMVERTEX, D3DPOOL_MANAGED, &m_pVertex, NULL );

	void* pointer = nullptr;
	m_pVertex->Lock(0, 0, &pointer, 0);
	::memcpy(pointer, &p_Vertex.data()[0], length);
	m_pVertex->Unlock();
}

void Mesh::SetIndexBuffer(vector<uint16>& p_Index)
{
	m_iIndexSize = p_Index.size();
	int32 length = m_iIndexSize * sizeof(uint16);
	DEVICE->CreateIndexBuffer(length, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIndex, nullptr);

	void* pointer = nullptr;
	m_pIndex->Lock(0, 0, &pointer, 0);
	::memcpy(pointer, p_Index.data(), length);
	m_pIndex->Unlock();
}

