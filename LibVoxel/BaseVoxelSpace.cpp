#include "BaseVoxelSpace.h"
#include "Utils.h"
#include <Math/MathUtil.h>
BaseVoxelSpace::BaseVoxelSpace()
	: m_fCellSize(0.0f)
	, m_iXSize(0)
	, m_iYSize(0)
	, m_iZSize(0)
	, m_Voexl(nullptr)
{
}

BaseVoxelSpace::~BaseVoxelSpace()
{
	if (m_Voexl != nullptr)
	{
		delete[] m_Voexl;
	}
}

bool BaseVoxelSpace::Init(const Vector3 & vMin, const Vector3 & vMax, float fCellSize)
{
	m_BoundBox.min = vMin;
	m_BoundBox.max = vMax;
	m_iXSize = uint32_t((vMax.x - vMin.y) / fCellSize + 1);
	m_iYSize = uint32_t((vMax.y - vMin.y) / fCellSize + 1);
	m_iZSize = uint32_t((vMax.z - vMin.z) / fCellSize + 1);
	if (m_iXSize == 0 || m_iYSize == 0 || m_iZSize == 0 )
	{
		LOG_ERROR(" please check boundbox !");
		return false;
	}
	m_Voexl = new uint8_t[m_iXSize*m_iYSize*m_iZSize];
	memset(m_Voexl, 0, m_iXSize*m_iYSize*m_iZSize);

	return true;
}

void BaseVoxelSpace::AddTriangle(const Vector3 & vP0, const Vector3 & vP1, const Vector3 & vP2)
{
	float fLen = 0.0f;
	{
		float fLenTemp = distance(vP1, vP0);
		if (fLenTemp > fLen) fLen = fLenTemp;
	}
	{
		float fLenTemp = distance(vP2, vP1);
		if (fLenTemp > fLen) fLen = fLenTemp;
	}
	{
		float fLenTemp = distance(vP0, vP2);
		if (fLenTemp > fLen) fLen = fLenTemp;
	}
	uint32_t iMaxSegment = uint32_t(fLen / m_fCellSize + 0.99);//计算边分割最大片段
	if (iMaxSegment <= 1)
	{
		AddPoint(vP0);
		AddPoint(vP1);
		AddPoint(vP2);
		return;
	}
	for (uint32_t i=0; i<=iMaxSegment; ++i)
	{

	}

}

bool BaseVoxelSpace::AddPoint(const Vector3 & vPoint)
{
	if (!m_BoundBox.contains(vPoint))
		return false;
	
	Vector3 vIndex = (vPoint - m_BoundBox.min);
	float fOneOverCellSize = 1 / m_fCellSize;
	vIndex *= fOneOverCellSize;
	vIndex += Vector3(0.99f, 0.99f, 0.99f);//向上取整
	uint32_t iX = clamp((uint32_t)vIndex.x, 0U, m_iXSize) - 1;
	uint32_t iY = clamp((uint32_t)vIndex.y, 0U, m_iYSize) - 1;
	uint32_t iZ = clamp((uint32_t)vIndex.z, 0U, m_iZSize) - 1;

	SetVoxelType(iX, iY, iZ, eBVT_TriMark);
	return true;
}

BaceVoxelType BaseVoxelSpace::GetVoexlType(uint32_t iX, uint32_t iY, uint32_t iZ)
{
	if (iX > m_iXSize || iY > m_iYSize || iZ > m_iZSize)
	{
		LOG_ERROR("out range");
		return eBVT_NULL;
	}

	return BaceVoxelType(m_Voexl[iX + iY*m_iYSize + iZ * m_iZSize]);
}

void BaseVoxelSpace::SetVoxelType(uint32_t iX, uint32_t iY, uint32_t iZ, BaceVoxelType eType)
{
	if (iX > m_iXSize || iY > m_iYSize || iZ > m_iZSize)
	{
		LOG_ERROR("out range");
		return;
	}
	m_Voexl[iX + iY*m_iYSize + iZ * m_iZSize] = (uint8_t)eType;
}
	