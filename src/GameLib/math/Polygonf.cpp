#include "Polygonf.h"
#include "Rect2f.h"
#include "MathLib.h"


//-----------------------------------------------------------------------------
// Constructors.
//-----------------------------------------------------------------------------

Polygonf::Polygonf()
{
}

Polygonf::Polygonf(const Rect2f& rect)
{
	m_vertices.push_back(rect.GetTopLeft());
	m_vertices.push_back(rect.GetTopRight());
	m_vertices.push_back(rect.GetBottomRight());
	m_vertices.push_back(rect.GetBottomLeft());
}


//-----------------------------------------------------------------------------
// Accessors.
//-----------------------------------------------------------------------------

Vector2f Polygonf::GetNormal(int index) const
{
	int n = (int) m_vertices.size();
	Vector2f normal = m_vertices[index] - m_vertices[(index + n - 1) % n];
	float temp = normal.x;
	normal.x = -normal.y;
	normal.y = temp;
	normal.Normalize();
	return normal;
}


//-----------------------------------------------------------------------------
// Mutators.
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Static methods.
//-----------------------------------------------------------------------------
