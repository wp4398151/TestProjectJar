#pragma once

#include <d3d11.h>
#include <d3dx10math.h>
#include "Common.h"

class AxisModelClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR4 color;
	};

public:
	AxisModelClass(void);
	AxisModelClass(const AxisModelClass &);
	~AxisModelClass(void);
	bool Initialize(ID3D11Device*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();

	void GetWorldMatrix(D3DXMATRIX& m_WorldMatrix);
private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);
	//¶¥µã»º³åºÍ¶¥µãË÷Òý»º³å
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	D3DXMATRIX m_WorldMatrix;
	int m_vertexCount, m_indexCount;
};

