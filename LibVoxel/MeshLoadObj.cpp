#include "MeshLoadObj.h"
#include <fstream>
#include "Utils.h"

MeshLoadObj::MeshLoadObj()
	: m_fScale(1.0f)
	, m_bAABBReset(false)
{
	m_AABB.empty();
}


MeshLoadObj::~MeshLoadObj()
{
}

bool MeshLoadObj::Load(const std::string & fileName)
{
	std::vector<char> vData;
	if (!LoadBuffer(fileName, vData))
		return false;

	char* src = &(vData[0]);
	char* srcEnd = src + vData.size();
	char row[512];
	int32_t face[32];
	float x, y, z;
	int nv;
	int tcap = 0;
	while (src < srcEnd)
	{
		// Parse one row
		row[0] = '\0';
		src = ParseRow(src, srcEnd, row, sizeof(row) / sizeof(char));
		// Skip comments
		if (row[0] == '#') continue;
		if (row[0] == 'v' && row[1] != 'n' && row[1] != 't')
		{
			// Vertex pos
			sscanf_s(row + 1, "%f %f %f", &x, &y, &z);
			AddVertex(x, y, z);
		}
		if (row[0] == 'f')
		{
			// Faces
			nv = ParseFace(row + 1, face, 32);
			for (int i = 2; i < nv; ++i)
			{
				const int32_t a = face[0];
				const int32_t b = face[i - 1];
				const int32_t c = face[i];
				if (a < 0 || a >= (int32_t)m_vVerts.size() 
					|| b < 0 || b >= (int32_t)m_vVerts.size() 
					|| c < 0 || c >= (int32_t)m_vVerts.size())
					continue;
				AddTriangle(a, b, c);
			}
		}
	}

	// Calculate normals.
	m_vNormals.reserve(m_vTris.size());
	for (size_t i=0; i< m_vTris.size(); i += 3)
	{
		const Vector3& v0 = m_vVerts[m_vTris[i]];
		const Vector3& v1 = m_vVerts[m_vTris[i+1]];
		const Vector3& v2 = m_vVerts[m_vTris[i+2]];

		Vector3 e0 = v1 - v2;
		Vector3 e1 = v2 - v0;
		Vector3 normal = crossProduct(e0, e1);
		normal.normalize();
		m_vNormals.push_back(normal);
	}
	m_strFileName = fileName;
	return true;
}

Vector3 MeshLoadObj::Center()
{
	return m_AABB.center();
}

Vector3 MeshLoadObj::Max()
{
	return m_AABB.max;
}

bool MeshLoadObj::LoadBuffer(const std::string & fileName, std::vector<char>& vData)
{
	std::basic_ifstream<char> stream(fileName, std::ios::binary);
	if (!stream)
	{
		LOG_ERROR("cannot open file" << fileName);
		return false;
	}
	stream.unsetf(std::ios::skipws);
	stream.seekg(0, std::ios::end);
	size_t iSize = (size_t)stream.tellg();
	stream.seekg(0);
	vData.resize(iSize + 1);
	stream.read(&vData.front(), static_cast<std::streamsize>(iSize));
	vData[iSize] = 0;
	return true;
}

char * MeshLoadObj::ParseRow(char * buf, char * bufEnd, char * row, int32_t len)
{
	bool start = true;
	bool done = false;
	int32_t n = 0;
	while (!done && buf < bufEnd)
	{
		char c = *buf;
		buf++;
		// multirow
		switch (c)
		{
		case '\\':
			break;
		case '\n':
			if (start) break;
			done = true;
			break;
		case '\r':
			break;
		case '\t':
		case ' ':
			if (start) break;
		default:
			start = false;
			row[n++] = c;
			if (n >= len - 1)
				done = true;
			break;
		}
	}
	row[n] = '\0';
	return buf;
}

int MeshLoadObj::ParseFace(char * row, int32_t * data, int32_t n)
{
	int32_t j = 0;
	while (*row != '\0')
	{
		// Skip initial white space
		while (*row != '\0' && (*row == ' ' || *row == '\t'))
			row++;
		char* s = row;
		// Find vertex delimiter and terminated the string there for conversion.
		while (*row != '\0' && *row != ' ' && *row != '\t')
		{
			if (*row == '/') *row = '\0';
			row++;
		}
		if (*s == '\0')
			continue;
		int32_t vi = atoi(s);
		data[j++] = vi < 0 ? vi + (int32_t)m_vVerts.size() : vi - 1;
		if (j >= n) return j;
	}
	return j;
}

void MeshLoadObj::AddVertex(float x, float y, float z)
{
	Vector3 vPoint;
	vPoint.x = x*m_fScale;
	vPoint.y = y*m_fScale;
	vPoint.z = z*m_fScale;
	m_vVerts.push_back(vPoint);

	if (!m_bAABBReset)
	{
		m_AABB.min = vPoint;
		m_AABB.max = vPoint;
		m_bAABBReset = true;
	}
	m_AABB.add(vPoint);
	
}

void MeshLoadObj::AddTriangle(int32_t a, int32_t b, int32_t c)
{
	m_vTris.push_back(a);
	m_vTris.push_back(b);
	m_vTris.push_back(c);
}
