#ifndef _POLYGON_F_H_
#define _POLYGON_F_H_

#include "Vector2f.h"
#include <vector>

struct Rect2f;


struct Polygonf
{
public:
	// Constructors.
	Polygonf();
	Polygonf(const Rect2f& rect);

	// Accessors.
	inline size_t size() const { return m_vertices.size(); }
	inline int GetVertexCount() const { return (int) m_vertices.size(); }
	inline Vector2f GetVertex(unsigned int index) const { return m_vertices[index]; }
	Vector2f GetNormal(int index) const;

	// Mutators.
	inline void AddVertex(const Vector2f& vertex) { m_vertices.push_back(vertex); }

	inline Vector2f& operator[](unsigned int index) { return m_vertices[index]; }
	inline Vector2f operator[](unsigned int index) const { return m_vertices[index]; }

private:
	std::vector<Vector2f> m_vertices;
};


#endif // _POLYGON_F_H_