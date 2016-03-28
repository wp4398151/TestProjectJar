#pragma once
#include <windows.h>
#include <D3D11.h>
#include <map>
#include <string>

using namespace std;

class TextureManager
{
public:
	TextureManager();
	~TextureManager();
	
	bool Initialize(ID3D11Device* pDevice);
	void Uninitialize();
	ID3D11ShaderResourceView* LoadTextureFromFile(ID3D11Device* pDevice, wstring &rWTextureFilePath);
	
	ID3D11SamplerState *GetSamplaState();
	
public:
	ID3D11SamplerState* m_pSamplerState;
	map<wstring, ID3D11ShaderResourceView*> m_TextureMap;
};

