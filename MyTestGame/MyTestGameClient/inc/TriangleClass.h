#pragma once

#include <d3d11.h>
#include <d3dx10math.h>
#include "TextureClass.h"

class TriangleClass
{
public:
	TriangleClass(void);
	TriangleClass(const TriangleClass &);

	~TriangleClass(void);
	bool Initialize(ID3D11Device* pDevice, WCHAR* wszTextureFileName);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();
private:
	bool LoadTexture(ID3D11Device* pDevice, WCHAR* pwzFileName);
	bool InitializeBuffers(ID3D11Device* pDevice);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);
	void ReleaseTexture();

	//¶¥µã»º³åºÍ¶¥µãË÷Òý»º³å
	ID3D11Buffer *m_pVertexBuffer, *m_pIndexBuffer;
	int m_vertexCount, m_indexCount;
	
	TextureClass *m_pTextureClass;
};