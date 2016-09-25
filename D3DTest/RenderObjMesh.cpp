#include "RenderObjMesh.h"
#include <Core/DXUT.h>
#include "RenderObjShader.h"
#include <Optional/DXUTcamera.h>
#include <MeshLoadObj.h>
using namespace DirectX;
RenderObjMesh::RenderObjMesh()
	: m_pIndexBuffer(nullptr)
	, m_pVerterBuffer(nullptr)
	, m_pRenderShader(nullptr)
	, m_iIndexCount(0)
	, m_bLoadOK(false)
{
}

RenderObjMesh::~RenderObjMesh()
{
	Destory();
}

bool RenderObjMesh::Create(ID3D11Device * pd3dDevice, MeshLoadObj * pObjLoad)
{
	HRESULT hr = S_OK;

	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pVerterBuffer);
	
	m_iIndexCount = (uint32_t)pObjLoad->Tris().size();

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Vector3) * pObjLoad->Verts().size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = &(pObjLoad->Verts()[0]);
	hr = pd3dDevice->CreateBuffer(&bd, &InitData, &m_pVerterBuffer);
	if (FAILED(hr))
		return false;

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(uint32_t) * m_iIndexCount;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	InitData.pSysMem = &(pObjLoad->Tris()[0]);
	hr = pd3dDevice->CreateBuffer(&bd, &InitData, &m_pIndexBuffer);
	if (FAILED(hr))
		return false;

	m_bLoadOK = true;
	return true;
}

void RenderObjMesh::SetRenderObjShader(RenderObjShader * pShader)
{
	m_pRenderShader = pShader;
}

void RenderObjMesh::OnRender(ID3D11DeviceContext * pd3dImmediateContext, CModelViewerCamera * pCamera)
{
	if (!m_bLoadOK)
		return;
	static DirectX::XMMATRIX s_defult = DirectX::XMMatrixIdentity();
	if (m_pRenderShader)
	{
		m_pRenderShader->OnRender(pd3dImmediateContext, pCamera->GetProjMatrix(), pCamera->GetViewMatrix(), s_defult);
// 		XMVECTOR Eye = XMVectorSet(0.0f, 1.0f, -5.0f, 0.0f);
// 		XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
// 		XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
// 		XMMATRIX g_View = XMMatrixLookAtLH(Eye, At, Up);
// 		XMMATRIX g_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, 800 / (FLOAT)600, 0.01f, 100.0f);
// 		m_pRenderShader->OnRender(pd3dImmediateContext, g_Projection, g_View, s_defult);
	}
	UINT stride = sizeof(float) * 3;
	UINT offset = 0;
	pd3dImmediateContext->IASetVertexBuffers(0, 1, &m_pVerterBuffer, &stride, &offset);
	pd3dImmediateContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pd3dImmediateContext->DrawIndexed(m_iIndexCount, 0, 0);
}

void RenderObjMesh::Destory()
{
	m_bLoadOK = false;
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pVerterBuffer);
}
