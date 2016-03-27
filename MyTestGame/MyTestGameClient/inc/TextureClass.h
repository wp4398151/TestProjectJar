#pragma once
#include <d3d11.h>
#include <D3DX11tex.h>

class TextureClass
{
public:
	TextureClass();
	TextureClass(const TextureClass& textureClass);
	~TextureClass();

	bool Initialize(ID3D11Device* pDevice, WCHAR *lpwFileName);
	bool Shutdown();
	ID3D11ShaderResourceView* GetTexutre();
private:
	ID3D11ShaderResourceView* m_pTextureView;
};