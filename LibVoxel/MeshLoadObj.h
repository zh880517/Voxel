#pragma once
#include <vector>
#include <string>
#include <Math/vector3.h>
#include <Math/AABB3.h>


class MeshLoadObj
{
public:
	MeshLoadObj();
	~MeshLoadObj();

	bool Load(const std::string& fileName);

	std::vector<Vector3>& Verts() { return m_vVerts; }
	std::vector<int32_t>& Tris() { return m_vTris; }

	Vector3 Center();

	Vector3 Max();

protected:
	bool	LoadBuffer(const std::string& fileName, std::vector<char>& vData);

	char*	ParseRow(char* buf, char* bufEnd, char* row, int32_t len);

	int		ParseFace(char* row, int32_t* data, int32_t n);

	void	AddVertex(float x, float y, float z);

	void	AddTriangle(int32_t a, int32_t b, int32_t c);

private:
	float					m_fScale;
	AABB3					m_AABB;
	bool					m_bAABBReset;
	std::vector<Vector3>	m_vVerts;
	std::vector<int32_t>	m_vTris;
	std::vector<Vector3>	m_vNormals;
	std::string				m_strFileName;
};

