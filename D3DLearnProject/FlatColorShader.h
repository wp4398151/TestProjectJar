#pragma once

#include <D3D11.h>

class FlatColorShader
{
public:
	FlatColorShader();
	~FlatColorShader();
	void Shutdown();
	bool Init(ID3D11Device* pDevice);
	bool Render(ID3D11DeviceContext *pDeviceContext, int indexCount);
	void RenderShader(ID3D11DeviceContext *pDeviceContext, int indexCount);
};

