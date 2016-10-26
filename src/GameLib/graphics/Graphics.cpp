#include "Graphics.h"
#include <math/MathLib.h>
#include <SDL2/SDL.h>
#include "OpenGLIncludes.h"


Graphics::Graphics(Window* window) :
	m_window(window)
{
}

void Graphics::Clear(const Color& color)
{
	Vector4f c = color.ToVector4f();
	glClearColor(c.x, c.y, c.z, c.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Graphics::SetViewport(const Viewport& viewport, bool scissor, bool flipY)
{
	int y = viewport.y;
	if (flipY)
		y = m_window->GetHeight() - (viewport.y + viewport.height);
	glViewport(viewport.x, y, viewport.width, viewport.height);
	if (scissor)
	{
		glEnable(GL_SCISSOR_TEST);
		glScissor(viewport.x, y, viewport.width, viewport.height);
	}
}


//-----------------------------------------------------------------------------
// Lines
//-----------------------------------------------------------------------------

void Graphics::DrawLine(float x1, float y1, float x2, float y2, const Color& color)
{
	glBegin(GL_LINES);
	gl_Color(color);
	glVertex2f(x1, y1);
	glVertex2f(x2, y2);
	glEnd();
}

void Graphics::DrawLine(const Vector2f& from, const Vector2f& to, const Color& color)
{
	glBegin(GL_LINES);
	gl_Color(color);
	glVertex2f(from.x, from.y);
	glVertex2f(to.x, to.y);
	glEnd();
}


//-----------------------------------------------------------------------------
// Rectangles
//-----------------------------------------------------------------------------

void Graphics::DrawRect(const Viewport& rect, const Color& color)
{
	glBegin(GL_LINE_LOOP);
	gl_Color(color);
	glVertex2i(rect.x, rect.y);
	glVertex2i(rect.x + rect.width, rect.y);
	glVertex2i(rect.x + rect.width, rect.y + rect.height);
	glVertex2i(rect.x, rect.y + rect.height);
	glEnd();
}

void Graphics::DrawRect(const Rect2f& rect, const Color& color)
{
	glBegin(GL_LINE_LOOP);
	gl_Color(color);
	glVertex2f(rect.position.x, rect.position.y);
	glVertex2f(rect.position.x + rect.size.x, rect.position.y);
	glVertex2f(rect.position.x + rect.size.x, rect.position.y + rect.size.y);
	glVertex2f(rect.position.x, rect.position.y + rect.size.y);
	glEnd();
}

void Graphics::DrawRect(const Vector2f& pos, const Vector2f& size, const Color& color)
{
	glBegin(GL_LINE_LOOP);
	gl_Color(color);
	glVertex2f(pos.x, pos.y);
	glVertex2f(pos.x + size.x, pos.y);
	glVertex2f(pos.x + size.x, pos.y + size.y);
	glVertex2f(pos.x, pos.y + size.y);
	glEnd();
}

void Graphics::DrawRect(float x, float y, float width, float height, const Color& color)
{
	glBegin(GL_LINE_LOOP);
	gl_Color(color);
	glVertex2f(x, y);
	glVertex2f(x + width, y);
	glVertex2f(x + width, y + height);
	glVertex2f(x, y + height);
	glEnd();
}

void Graphics::FillRect(const Vector2f& pos, const Vector2f& size, const Color& color)
{
	glBegin(GL_QUADS);
	gl_Color(color);
	glVertex2f(pos.x, pos.y);
	glVertex2f(pos.x + size.x, pos.y);
	glVertex2f(pos.x + size.x, pos.y + size.y);
	glVertex2f(pos.x, pos.y + size.y);
	glEnd();
}

void Graphics::FillRect(const Viewport& rect, const Color& color)
{
	glBegin(GL_QUADS);
	gl_Color(color);
	glVertex2i(rect.x, rect.y);
	glVertex2i(rect.x + rect.width, rect.y);
	glVertex2i(rect.x + rect.width, rect.y + rect.height);
	glVertex2i(rect.x, rect.y + rect.height);
	glEnd();
}

void Graphics::FillRect(const Rect2f& rect, const Color& color)
{
	glBegin(GL_QUADS);
	gl_Color(color);
	glVertex2f(rect.position.x, rect.position.y);
	glVertex2f(rect.position.x + rect.size.x, rect.position.y);
	glVertex2f(rect.position.x + rect.size.x, rect.position.y + rect.size.y);
	glVertex2f(rect.position.x, rect.position.y + rect.size.y);
	glEnd();
}

void Graphics::FillRect(float x, float y, float width, float height, const Color& color)
{
	glBegin(GL_QUADS);
	gl_Color(color);
	glVertex2f(x, y);
	glVertex2f(x + width, y);
	glVertex2f(x + width, y + height);
	glVertex2f(x, y + height);
	glEnd();
}


//-----------------------------------------------------------------------------
// Circles
//-----------------------------------------------------------------------------

void Graphics::DrawCircle(const Vector2f& pos, float radius, const Color& color, int numEdges)
{
	glBegin(GL_LINE_LOOP);
	gl_Color(color);
	for (int i = 0; i < numEdges; i++)
	{
		float angle = (i / (float) numEdges) * Math::TWO_PI;
		Vector2f v = pos + Vector2f(Math::Cos(angle) * radius, Math::Sin(angle) * radius);
		glVertex2fv(v.data());
	}
	glEnd();
}

void Graphics::FillCircle(const Vector2f& pos, float radius, const Color& color, int numEdges)
{
	glBegin(GL_TRIANGLE_FAN);
	gl_Color(color);
	for (int i = 0; i < numEdges; i++)
	{
		float angle = (i / (float) numEdges) * Math::TWO_PI;
		Vector2f v = pos + Vector2f(Math::Cos(angle) * radius, Math::Sin(angle) * radius);
		glVertex2fv(v.data());
	}
	glEnd();
}


//-----------------------------------------------------------------------------
// Polygons
//-----------------------------------------------------------------------------

void Graphics::DrawPolygon(const Polygonf& poly, const Color& color)
{
	glBegin(GL_LINE_LOOP);
	gl_Color(color);
	for (unsigned int i = 0; i < poly.size(); i++)
		glVertex2fv(poly[i].data());
	glEnd();
}

void Graphics::FillPolygon(const Polygonf& poly, const Color& color)
{
	// NOTE: This will only work correctly for convex polygons.
	glBegin(GL_TRIANGLE_FAN);
	gl_Color(color);
	for (unsigned int i = 0; i < poly.size(); i++)
		glVertex2fv(poly[i].data());
	glEnd();
}


//-----------------------------------------------------------------------------
// Textures
//-----------------------------------------------------------------------------

void Graphics::DrawTexture(Texture* pTexture, float x, float y, const Color& color)
{
	DrawTextureRegion(pTexture,
		0, 0, (float) pTexture->GetWidth(), (float) pTexture->GetHeight(),
		x, y, (float) pTexture->GetWidth(), (float) pTexture->GetHeight(),
		color);
}

void Graphics::DrawTexture(Texture* pTexture, const Vector2f& position, const Color& color)
{
	DrawTextureRegion(pTexture,
		0, 0, (float) pTexture->GetWidth(), (float) pTexture->GetHeight(),
		position.x, position.y, (float) pTexture->GetWidth(), (float) pTexture->GetHeight(),
		color);
}

void Graphics::DrawTexture(Texture* pTexture, float x, float y, float width, float height, const Color& color)
{
	DrawTextureRegion(pTexture,
		0, 0, (float) pTexture->GetWidth(), (float) pTexture->GetHeight(),
		x, y, width, height,
		color);
}

void Graphics::DrawTexture(Texture* pTexture, const Vector2f& position, const Vector2f& size, const Color& color)
{
	DrawTextureRegion(pTexture,
		0, 0, (float) pTexture->GetWidth(), (float) pTexture->GetHeight(),
		position.x, position.y, size.x, size.y,
		color);
}

void Graphics::DrawTexture(Texture* pTexture, const Rect2f& destination, const Color& color)
{
	DrawTextureRegion(pTexture,
		0, 0, (float) pTexture->GetWidth(), (float) pTexture->GetHeight(),
		destination.position.x, destination.position.y, destination.size.x, destination.size.y,
		color);
}

void Graphics::DrawTextureRegion(Texture* texture,
	float sourceX, float sourceY, float sourceWidth, float sourceHeight,
	float destX,   float destY,   float destWidth,   float destHeight,
	const Color& color)
{
	// Calculate texture coordinates.
	float texMinX = sourceX / (float)texture->GetWidth();
	float texMinY = sourceY / (float)texture->GetHeight();
	float texMaxX = (sourceX + sourceWidth) / (float)texture->GetWidth();
	float texMaxY = (sourceY + sourceHeight) / (float)texture->GetHeight();

	glBindTexture(GL_TEXTURE_2D, texture->GetGLTextureId());
	glBegin(GL_QUADS);
	gl_Color(color);
	glTexCoord2f(texMinX, texMinY);
	glVertex2f(destX, destY);
	glTexCoord2f(texMaxX, texMinY);
	glVertex2f(destX + destWidth, destY);
	glTexCoord2f(texMaxX, texMaxY);
	glVertex2f(destX + destWidth, destY + destHeight);
	glTexCoord2f(texMinX, texMaxY);
	glVertex2f(destX, destY + destHeight);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
}

//-----------------------------------------------------------------------------
// Fonts and Text
//-----------------------------------------------------------------------------

void Graphics::DrawString(SpriteFont* font, const char* text, const Vector2f& pos, const Color& color, float scale)
{
	font->DrawString(text, pos, color.ToVector4f(), scale);
}


// Draw colored text at a position.
void Graphics::DrawString(Font* pFont, const std::string& text, float x, float y, const Color& color, Align::value_type align)
{
	Texture* pTexture = pFont->GetGlyphTexture();
	Glyph glyph;

	// Update uniforms.
	/*m_pRenderContext->SetShader(m_spTextShader);
	m_pRenderContext->SetUniform(m_spTextShader->GetParam("matMVP"), m_pRenderContext->GetMVPMatrix());
	m_pRenderContext->SetUniform(m_spTextShader->GetParam("color"), color);
	m_pRenderContext->SetUniform(m_spTextShader->GetParam("sampler"), pTexture);*/

	Vector2f penPosition(x, y);

	if (align != Align::BOTTOM_LEFT)
	{
		Rect2f bounds;
		penPosition = Vector2f::ZERO;
		for (unsigned int i = 0; i < text.length(); i++)
		{
			glyph = pFont->GetGlyph(text[i]);

			Rect2f r(penPosition.x + glyph.GetMinX(),
					 penPosition.y + glyph.GetMinY(),
					 (float) (glyph.GetMaxX() - glyph.GetMinX()),
					 (float) (glyph.GetMaxY() - glyph.GetMinY()));
			if (i == 0)
				bounds = r;
			else
				bounds = Rect2f::GetUnion(bounds, r);

			penPosition.x += glyph.GetAdvance();
		}

		penPosition = Vector2f(x, y);
		if (align & Align::RIGHT)
			penPosition.x -= bounds.GetRight();
		else if (align & Align::CENTER)
			penPosition.x -= bounds.GetCenter().x;
		if (align & Align::TOP)
			penPosition.y -= bounds.GetTop();
		else if (align & Align::MIDDLE)
			penPosition.y -= bounds.GetCenter().y;
	}

	// Draw each glyph in the string of characters.
	for (unsigned int i = 0; i < text.length(); i++)
	{
		glyph = pFont->GetGlyph(text[i]);

		if (glyph.HasImage())
		{
			// Calculate texture coordinates.
			//float texMinX = glyph.GetSourceX() / (float)pTexture->GetWidth();
			//float texMinY = glyph.GetSourceY() / (float)pTexture->GetHeight();
			//float texMaxX = (glyph.GetSourceX() + glyph.GetWidth()) / (float)pTexture->GetWidth();
			//float texMaxY = (glyph.GetSourceY() + glyph.GetHeight()) / (float)pTexture->GetHeight();

			//// Setup vertices.
			//float destX = penPosition.x + glyph.GetMinX();
			//float destY = penPosition.y + glyph.GetMinY();
			//float destWidth = (float)(glyph.GetMaxX() - glyph.GetMinX());
			//float destHeight = (float)(glyph.GetMaxY() - glyph.GetMinY());

			//m_vertexList[0].position.Set(destX, destY, 0.0f);
			//m_vertexList[1].position.Set(destX + destWidth, destY, 0.0f);
			//m_vertexList[2].position.Set(destX + destWidth, destY + destHeight, 0.0f);
			//m_vertexList[3].position.Set(destX, destY + destHeight, 0.0f);
			//m_vertexList[0].texCoord.Set(texMinX, texMinY);
			//m_vertexList[1].texCoord.Set(texMaxX, texMinY);
			//m_vertexList[2].texCoord.Set(texMaxX, texMaxY);
			//m_vertexList[3].texCoord.Set(texMinX, texMaxY);
			//m_vertexBuffer.SetVertices(4, m_vertexList);

			//// Draw the vertices.
			//m_pRenderContext->DrawArrays(PRIMITIVES_QUADS, &m_vertexBuffer, 0, 4);

			
			DrawTextureRegion(pTexture,
				(float) glyph.GetSourceX(),
				(float) glyph.GetSourceY(),
				(float) glyph.GetWidth(),
				(float) glyph.GetHeight(),
				(penPosition.x + glyph.GetMinX()),
				(penPosition.y + glyph.GetMinY()),
				(float) (glyph.GetMaxX() - glyph.GetMinX()),
				(float) (glyph.GetMaxY() - glyph.GetMinY()),
				color);
		}

		// Advance the pen position.
		penPosition.x += glyph.GetAdvance();
	}
}


//-----------------------------------------------------------------------------
// OpenGL Helper Functions
//-----------------------------------------------------------------------------

void Graphics::gl_Vertex(const Vector2f& v)
{
	glVertex2fv(v.data());
}

void Graphics::gl_Vertex(const Vector3f& v)
{
	glVertex3fv(v.data());
}

void Graphics::gl_Color(const Color& color)
{
	glColor4ubv(color.data());
}

void Graphics::EnableCull(bool cull)
{
	if (cull)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
}

void Graphics::EnableDepthTest(bool depthTest)
{
	if (depthTest)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
}

void Graphics::SetProjection(const Matrix4f& projection)
{
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(projection.GetTranspose().data());
	glMatrixMode(GL_MODELVIEW);
}

void Graphics::ResetTransform()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void Graphics::SetTransform(const Matrix4f& transform)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(transform.GetTranspose().data());
}

void Graphics::Transform(const Matrix4f& transform)
{
	glMatrixMode(GL_MODELVIEW);
	glMultMatrixf(transform.GetTranspose().data());
}

void Graphics::Rotate(const Vector3f& axis, float angle)
{
	glMatrixMode(GL_MODELVIEW);
	glRotatef(angle * Math::RAD_TO_DEG, axis.x, axis.y, axis.z);
}

void Graphics::Rotate(const Quaternion& rotation)
{
	Transform(Matrix4f::CreateRotation(rotation));
}

void Graphics::Translate(const Vector2f& translation)
{
	glMatrixMode(GL_MODELVIEW);
	glTranslatef(translation.x, translation.y, 0.0f);
}

void Graphics::Translate(const Vector3f& translation)
{
	glMatrixMode(GL_MODELVIEW);
	glTranslatef(translation.x, translation.y, translation.z);
}

void Graphics::Scale(float scale)
{
	glMatrixMode(GL_MODELVIEW);
	glScalef(scale, scale, scale);
}

void Graphics::Scale(const Vector3f& scale)
{
	glMatrixMode(GL_MODELVIEW);
	glScalef(scale.x, scale.y, scale.z);
}
