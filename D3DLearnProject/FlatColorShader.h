#pragma once

#include <D3D11.h>
#include <D3DX10math.h>

class FlatColorShader
{
public:
	FlatColorShader();
	~FlatColorShader();
	void Shutdown();
	bool Init(ID3D11Device* pDevice);
	bool Render(ID3D11DeviceContext *pDeviceContext, int indexCount);
	void RenderShader(ID3D11DeviceContext *pDeviceContext, int indexCount);

private:
	ID3D11InputLayout *m_pLayout;
	ID3D11VertexShader *m_pVertexShader;
	ID3D11PixelShader *m_pPixelShader;
};

class FlatTriangle
{
public:
	FlatTriangle();
	struct VertexType{
		D3DXVECTOR3 pos;
		D3DXVECTOR4 color;
	};
	bool Initialize(ID3D11Device* pDevice);
	void Shutdown();

	void Render(ID3D11DeviceContext* pDeviceContext);
	int GetIndexCount();
	bool InitializeBuffers(ID3D11Device* pDevice);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext* pDeviceContext);

private:
	//¶¥µã»º³åºÍ¶¥µãË÷Òý»º³å
	ID3D11Buffer *m_pVertexBuffer, *m_pIndexBuffer;
	int m_vertexCount, m_indexCount;
};