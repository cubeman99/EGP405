#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <graphics/Color.h>
#include <graphics/Font.h>
#include <graphics/SpriteFont.h>
#include <graphics/Texture.h>
#include <graphics/Window.h>
#include <math/Matrix4f.h>
#include <math/Polygonf.h>
#include <math/Quaternion.h>
#include <math/Rect2f.h>
#include <math/Vector2f.h>
#include <math/Vector3f.h>
#include <math/Vector4f.h>
#include <assert.h>


struct Align
{
	enum
	{
		BOTTOM	= 0x0,
		LEFT	= 0x0,
		CENTER	= 0x1,
		RIGHT	= 0x2,
		MIDDLE	= 0x4,
		TOP		= 0x8,

		TOP_LEFT		= TOP | LEFT,
		TOP_RIGHT		= TOP | RIGHT,
		TOP_CENTER		= TOP | CENTER,
		BOTTOM_LEFT		= BOTTOM | LEFT,
		BOTTOM_RIGHT	= BOTTOM | RIGHT,
		BOTTOM_CENTER	= BOTTOM | CENTER,
		MIDDLE_LEFT		= MIDDLE | LEFT,
		MIDDLE_RIGHT	= MIDDLE | RIGHT,
		CENTERED		= CENTER | MIDDLE,
	};
	typedef int value_type;
};


struct Viewport
{
	int x;
	int y;
	int width;
	int height;

	Viewport()
	{}

	Viewport(int x, int y, int width, int height)
		: x(x), y(y), width(width), height(height)
	{}

	float GetAspectRatio() const
	{
		assert(height != 0);
		return ((float) width / (float) height);
	}

	void Inset(int amount)
	{
		x += amount;
		y += amount;
		width  -= amount * 2;
		height -= amount * 2;
	}

	void Inset(int left, int top, int right, int bottom)
	{
		x += left;
		y += top;
		width -= left + right;
		height -= top + bottom;
	}

	bool Contains(int x, int y)
	{
		return (x >= this->x && y >= this->y &&
				x < this->x + this->width &&
				y < this->y + this->height);
	}
};


class Graphics
{
public:
	Graphics(Window* window);

	// General render functions.
	void Clear(const Color& color);

	// Lines.
	void DrawLine(float x1, float y1, float x2, float y2, const Color& color);
	void DrawLine(const Vector2f& from, const Vector2f& to, const Color& color);

	// Rectangles.
	void DrawRect(const Viewport& rect, const Color& color);
	void DrawRect(const Rect2f& rect, const Color& color);
	void DrawRect(const Vector2f& pos, const Vector2f& size, const Color& color);
	void DrawRect(float x, float y, float width, float height, const Color& color);
	void FillRect(const Vector2f& pos, const Vector2f& size, const Color& color);
	void FillRect(const Viewport& rect, const Color& color);
	void FillRect(const Rect2f& rect, const Color& color);
	void FillRect(float x, float y, float width, float height, const Color& color);

	// Circles.
	void DrawCircle(const Vector2f& pos, float radius, const Color& color, int numEdges = 20);
	void FillCircle(const Vector2f& pos, float radius, const Color& color, int numEdges = 20);

	// Polygons.
	void DrawPolygon(const Polygonf& poly, const Color& color);
	void FillPolygon(const Polygonf& poly, const Color& color);

	// Textures.
	void DrawTexture(Texture* pTexture, float x, float y,
		const Color& color = Color::WHITE);
	void DrawTexture(Texture* pTexture, const Vector2f& position,
		const Color& color = Color::WHITE);
	void DrawTexture(Texture* pTexture, float x, float y, float width,
		float height, const Color& color = Color::WHITE);
	void DrawTexture(Texture* pTexture, const Vector2f& position,
		const Vector2f& size, const Color& color = Color::WHITE);
	void DrawTexture(Texture* pTexture, const Rect2f& destination,
		const Color& color = Color::WHITE);
	void DrawTextureRegion(Texture* pTexture,
		float sourceX, float sourceY, float sourceWidth, float sourceHeight,
		float destX, float destY, float destWidth, float destHeight,
		const Color& color = Color::WHITE);
	//void DrawTextureRegion(Texture* pTexture, Rect2f& source,
		//Rect2f& destination, const Color& color = Color::WHITE);

	// Text and Fonts.
	void DrawString(SpriteFont* font, const char* text, const Vector2f& pos,
		const Color& color, float scale = 1.0f);
	void DrawString(Font* font, const std::string& text, float x, float y,
		const Color& color, Align::value_type align = Align::TOP_LEFT);

	// Render Settings.
	void SetViewport(const Viewport& viewport, bool scissor, bool flipY = true);
	void SetProjection(const Matrix4f& projection);
	void EnableCull(bool cull);
	void EnableDepthTest(bool depthTest);
	
	// Transformations.
	void ResetTransform();
	void SetTransform(const Matrix4f& transform);
	void Transform(const Matrix4f& transform);
	void Rotate(const Vector3f& axis, float angle);
	void Rotate(const Quaternion& rotation);
	void Translate(const Vector2f& translation);
	void Translate(const Vector3f& translation);
	void Scale(float scale);
	void Scale(const Vector3f& scale);

private:
	// OpenGL helper functions.
	void gl_Vertex(const Vector2f& v);
	void gl_Vertex(const Vector3f& v);
	void gl_Color(const Color& color);

	Window* m_window;
};


#endif // _GRAPHICS_H_