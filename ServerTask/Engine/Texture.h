#pragma once




class Texture
{
public:
	void SetTexture(const LPDIRECT3DTEXTURE9& p_tex);
	const LPDIRECT3DTEXTURE9& GetTexture() { return m_pTexture; }

private:
	wstring m_strName = L"";
	LPDIRECT3DTEXTURE9 m_pTexture;
};

