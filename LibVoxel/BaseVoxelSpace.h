#pragma once
#include <Math/AABB3.h>
#include <vector>

enum BaceVoxelType
{
	eBVT_NULL = 0,//初始状态
	eBVT_TriMark = 1,//三角形面重合
};

class BaseVoxelSpace
{
public:
	BaseVoxelSpace();
	~BaseVoxelSpace();

	bool	Init(const Vector3& vMin, const Vector3& vMax, float fCellSize);

	void	AddTriangle(const Vector3& vP0, const Vector3& vP1, const Vector3& vP2);

	bool	AddPoint(const Vector3& vPoint);

	BaceVoxelType GetVoexlType(uint32_t iX, uint32_t iY, uint32_t iZ);

protected:

	void	SetVoxelType(uint32_t iX, uint32_t iY, uint32_t iZ, BaceVoxelType eType);

private:
	uint8_t*	m_Voexl;
	AABB3		m_BoundBox;
	float		m_fCellSize;
	uint32_t	m_iXSize;
	uint32_t	m_iYSize;
	uint32_t	m_iZSize;
};
