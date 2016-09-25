#pragma once
#include <d3d11.h>
#include <vector>

class CModelViewerCamera;
class RenderObjShader;
class MeshLoadObj;
class RenderObjMesh
{
public:
	RenderObjMesh();
	~RenderObjMesh();

	bool	Create(ID3D11Device* pd3dDevice, MeshLoadObj* pObjLoad);

	void	SetRenderObjShader(RenderObjShader* pShader);

	void	OnRender(ID3D11DeviceContext* pd3dImmediateContext, CModelViewerCamera* pCamera);

	void	Destory();

private:
	bool				m_bLoadOK;
	ID3D11Buffer*		m_pVerterBuffer;
	ID3D11Buffer*		m_pIndexBuffer;
	RenderObjShader*	m_pRenderShader;
	uint32_t			m_iIndexCount;
};
