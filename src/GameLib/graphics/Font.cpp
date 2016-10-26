#include "Font.h"
//#include "DistanceTransform.h"
#include <graphics/OpenGLIncludes.h>
#include <string>
#include <math.h>
#include <assert.h>


//-----------------------------------------------------------------------------
// Glyph
//-----------------------------------------------------------------------------

// Initialize the glyph metrics.
Glyph::Glyph(FT_GlyphSlot pFTGlyph, unsigned long charCode)
{
	m_sourceX		= 0;
	m_sourceY		= 0;
	m_minX			= pFTGlyph->bitmap_left;
	m_minY			= -pFTGlyph->bitmap_top;
	m_width			= pFTGlyph->bitmap.width;
	m_height		= pFTGlyph->bitmap.rows;
	m_advance		= pFTGlyph->advance.x / 64;
	m_charCode		= charCode;
	m_pImageData	= NULL;
}



//-----------------------------------------------------------------------------
// FreeType library
//-----------------------------------------------------------------------------

FT_Library Font::msFreeTypeLibrary = NULL;


// Initialize the Free Type Library, returning whether it was successful.
bool Font::InitFreeTypeLibrary()
{
	if (msFreeTypeLibrary == NULL)
	{
		FT_Error ftError = FT_Init_FreeType(&msFreeTypeLibrary);

		if (ftError != FT_Err_Ok)
		{
			fprintf(stderr, "Error: failure to initialize FreeType library\n");
			return false;
		}
	}
	return true;
}

// Done using the Free Type Library.
void Font::QuitFreeTypeLibrary()
{
	if (msFreeTypeLibrary == NULL)
		FT_Done_FreeType(msFreeTypeLibrary);
}

// Load a font from file using the given size.
Font* Font::LoadFont(const std::string& fileName, int size)
{
	assert(msFreeTypeLibrary != NULL);
	FT_Face ftFace;
	FT_Error ftError;

	// Load the font face.
	ftError = FT_New_Face(msFreeTypeLibrary, fileName.c_str(), 0, &ftFace);
	if (ftError == FT_Err_Unknown_File_Format)
	{
		fprintf(stderr, "Error: unsupported font file format for %s\n", fileName.c_str());
		return NULL;
	}
	else if (ftError != FT_Err_Ok)
	{
		fprintf(stderr, "Error: failure to load font file %s\n", fileName.c_str());
		return NULL;
	}
	
	// Set the font size.
	ftError = FT_Set_Char_Size(ftFace, 0, size * 64, 96, 96);
	if (ftError != FT_Err_Ok)
	{
		fprintf(stderr, "Error: the font %s cannot support a size of %d\n", fileName.c_str(), size);
		FT_Done_Face(ftFace);
		return NULL;
	}

	// Create the font object.
	return new Font(ftFace, size);
}

// Load a font from file, using the given size and character region.
Font* Font::LoadFont(const std::string& fileName, int size, unsigned long charRegionBegin, unsigned long charRegionEnd)
{
	Font* pFont = LoadFont(fileName, size);
	if (pFont != NULL)
	{
		pFont->LoadGlyphRange(charRegionBegin, charRegionEnd);
		pFont->CreateGlyphAtlas();
	}
	return pFont;
}

Font* Font::LoadDistanceFieldFont(const std::string& fileName, int size,
			unsigned long charRegionBegin, unsigned long charRegionEnd, int scale, float spread)
{
	Font* pFont = LoadFont(fileName, size);
	if (pFont != NULL)
	{
		// TODO: actually use distance fields for this.
		pFont->LoadGlyphRange(charRegionBegin, charRegionEnd);
		pFont->CreateGlyphAtlas();
	}
	return pFont;
}



//-----------------------------------------------------------------------------
// Font constructors & destructor
//-----------------------------------------------------------------------------

// Create a font with the given FreeType face and size.
Font::Font(FT_Face pFace, int size) :
	m_familyName(pFace->family_name),
	m_styleName(pFace->style_name),
	m_size(size),
	m_ftFace(pFace),
	m_pixelsPerEM((int) pFace->size->metrics.x_ppem, (int) pFace->size->metrics.y_ppem),
	m_pGlyphAtlasTexture(NULL)
{
}

// Create a font with a distance field texture.
Font::Font(FT_Face pFace, int size, unsigned long charRegionBegin, unsigned long charRegionEnd, int scale, float spread) :
	m_familyName(pFace->family_name),
	m_styleName(pFace->style_name),
	m_size(size),
	m_ftFace(pFace),
	m_pixelsPerEM((int) pFace->size->metrics.x_ppem, (int) pFace->size->metrics.y_ppem),
	m_pGlyphAtlasTexture(NULL)
{
	/*
	m_charRegion.begin = charRegionBegin;
	m_charRegion.length = charRegionEnd - charRegionBegin;
	m_pGlyphs = new Glyph[m_charRegion.length];
	
	float** pGlyphImageData = new float*[m_charRegion.length];
	ShelfBinPacker rectPacker;
	FT_Error ftError;
	FT_ULong charCode;
	DistanceTransform distanceTransform;

	// Create the glyphs.
	for (unsigned long i = 0; i < m_charRegion.length; i++)
	{
		charCode = (FT_ULong) (m_charRegion.begin + i);
		ftError  = FT_Load_Char(pFace, charCode, FT_LOAD_RENDER);
		
		pGlyphImageData[i] = NULL;

		if (ftError == FT_Err_Ok)
		{
			Glyph& glyph = m_pGlyphs[i];
			FT_GlyphSlot g = pFace->glyph;
			glyph.init(g);
			
			int glyphArea = glyph.getWidth() * glyph.getHeight();
			int glyphImgArea = (glyph.getWidth() + spread*2) * (glyph.getHeight() + spread*2);

			if (pFace->glyph->bitmap.buffer != NULL)
			{
				// Copy the glyph's pixels into a pixel buffer.
				pGlyphImageData[i] = new float[glyphImgArea];
				//for (int j = 0; j < glyphArea; j++)
				//	pGlyphImageData[i][j] = pFace->glyph->bitmap.buffer[j];

				distanceTransform.CreateDistanceMap(glyph.getWidth(), glyph.getHeight(),
					pFace->glyph->bitmap.buffer, 128, spread, pGlyphImageData[i], spread);

				rectPacker.Insert(&glyph.m_sourceX, &glyph.m_sourceY, glyph.getWidth() + (spread * 2), glyph.getHeight() + (spread * 2), 1);
			}
		}
		else
		{
			std::cerr << "Error: undefined character code " << i << "" << std::endl;
		}
	}

	// Pack the glyphs rectangles with the shelf bin-packing algorithm.
	int texWidth  = 512;
	int texHeight = 512;
	rectPacker.Pack(&texWidth, &texHeight);

	// Create the glyph atlas texture.
	m_pGlyphAtlasTexture = new Texture();
	m_pGlyphAtlasTexture->m_width  = texWidth;
	m_pGlyphAtlasTexture->m_height = texHeight;
	m_pGlyphAtlasTexture->m_glTarget = GL_TEXTURE_2D;

	unsigned char* defaultColor = new unsigned char[texWidth * texHeight];
	memset(defaultColor, 255, texWidth * texHeight);

	glBindTexture(GL_TEXTURE_2D, m_pGlyphAtlasTexture->m_glTextureId);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Pixel alignment defaults to 4, but we are using two 1-byte components (lum & alpha)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);		// Use linear filtering for minimization.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);		// Use linear filtering for magnification.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texWidth, texHeight, 0, GL_RED, GL_UNSIGNED_BYTE, defaultColor);
	
	delete [] defaultColor;

	// Blit glyph images into the texture sheet.
	for (unsigned long i = 0; i < m_charRegion.length; i++)
	{
		if (pGlyphImageData[i] != NULL)
		{
			Glyph& glyph = m_pGlyphs[i];
			glTexSubImage2D(GL_TEXTURE_2D,
							0,
							glyph.getSourceX(),
							glyph.getSourceY(),
							glyph.getWidth() + (spread * 2),
							glyph.getHeight() + (spread * 2),
							GL_RED,
							GL_FLOAT,
							pGlyphImageData[i]);
			glyph.m_sourceX += spread;
			glyph.m_sourceY += spread;
			delete [] pGlyphImageData[i];
		}
	}
	m_pGlyphAtlasTexture->UpdateTextureParameters();
	m_pGlyphAtlasTexture->GenerateMipmaps();

	glBindTexture(GL_TEXTURE_2D, 0);
	delete [] pGlyphImageData;
	FT_Done_Face(pFace);
	*/
}

Font::~Font()
{
	// Delete all glyphs.
	for (GlyphMap::iterator it = m_glyphMap.begin(); it != m_glyphMap.end(); it++)
		delete it->second;
	if (m_pGlyphAtlasTexture != NULL)
		delete m_pGlyphAtlasTexture;
	if (m_ftFace != NULL)
		FT_Done_Face(m_ftFace);
}



//-----------------------------------------------------------------------------
// Accesors
//-----------------------------------------------------------------------------

// Return the glyph that represents the given character.
Glyph Font::GetGlyph(char c) const
{
	return GetGlyph((unsigned long) c);
}

// Return the glyph that represents the given character code.
Glyph Font::GetGlyph(unsigned long charCode) const
{
	return *m_glyphMap.at(charCode);
}

// Is the given character code defined in the font's character regions?
bool Font::IsCharDefined(unsigned long charCode) const
{
	GlyphMap::const_iterator it = m_glyphMap.find(charCode);
	return (m_glyphMap.find(charCode) != m_glyphMap.end());
}



//-----------------------------------------------------------------------------
// Glyph Loading & Glyph Atlas Creation
//-----------------------------------------------------------------------------

// Load a single glyph into the font.
void Font::LoadGlyph(unsigned long charCode)
{
	FT_Error ftError = FT_Load_Char(m_ftFace, charCode, FT_LOAD_RENDER);
	
	if (ftError == FT_Err_Ok)
	{
		// Create the glyph object.
		Glyph* pGlyph = new Glyph(m_ftFace->glyph, charCode);
		m_glyphMap[charCode] = pGlyph;

		if (m_ftFace->glyph->bitmap.buffer != NULL)
		{
			// Copy the glyph's pixels into a luminance/alpha buffer.
			int glyphArea = pGlyph->GetWidth() * pGlyph->GetHeight();
			pGlyph->m_pImageData = new unsigned char[glyphArea];
			for (int j = 0; j < glyphArea; j++)
				pGlyph->m_pImageData[j] = m_ftFace->glyph->bitmap.buffer[j]; // Alpha

			// Add the glyph's image bounds to the glyph packer.
			m_glyphPacker.Insert(&pGlyph->m_sourceX, &pGlyph->m_sourceY, pGlyph->GetWidth(), pGlyph->GetHeight(), 1);
		}
	}
	else
	{
		fprintf(stderr, "Error: undefined character code %lu\n", charCode);
	}
}

// Load a range of glpyhs into the font (inclusive start to exclusive end).
void Font::LoadGlyphRange(unsigned long rangeStart, unsigned long rangeEnd)
{
	for (unsigned long i = rangeStart; i < rangeEnd; i++)
		LoadGlyph(i);
}

// Create the glyph atlas texture containing all of the font's loaded glyphs.
void Font::CreateGlyphAtlas()
{
	if (m_pGlyphAtlasTexture != NULL)
	{
		delete m_pGlyphAtlasTexture;
		m_pGlyphAtlasTexture = NULL;
	}

	// Pack the glyphs rectangles with the shelf bin-packing algorithm.
	int texWidth, texHeight;
	m_glyphPacker.Pack(&texWidth, &texHeight);

	// Create the glyph atlas texture.
	m_pGlyphAtlasTexture = new Texture();
	m_pGlyphAtlasTexture->m_width = texWidth;
	m_pGlyphAtlasTexture->m_height = texHeight;
	
	// Create a solid-color texture.
	//unsigned char* defaultColor = new unsigned char[texWidth * texHeight];
	//memset(defaultColor, 0, texWidth * texHeight);
	//glBindTexture(GL_TEXTURE_2D, m_pGlyphAtlasTexture->GetGLTextureId());
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Pixel alignment defaults to 4, but we are using 1-byte components
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texWidth, texHeight, 0, GL_RED, GL_UNSIGNED_BYTE, defaultColor);
	//delete [] defaultColor;

	// TEMP: Create a transparent texture. TODO: Change this later.
	unsigned char* defaultColor = new unsigned char[texWidth * texHeight * 4];
	memset(defaultColor, 0, texWidth * texHeight * 4);
	glBindTexture(GL_TEXTURE_2D, m_pGlyphAtlasTexture->GetGLTextureId());
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Pixel alignment defaults to 4, but we are using 1-byte components
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, defaultColor);
	delete [] defaultColor;
	
	std::vector<unsigned char> glyphImageData; // TEMP: to make texture a white, alpha-texture

	// Blit glyph images into the texture sheet.
	for (GlyphMap::iterator it = m_glyphMap.begin(); it != m_glyphMap.end(); it++)
	{
		Glyph* pGlyph = it->second;
		if (pGlyph->m_pImageData != NULL)
		{
			glyphImageData.resize(pGlyph->GetWidth() * pGlyph->GetHeight() * 4);
			memset(glyphImageData.data(), 255, glyphImageData.size());
			for (unsigned int i = 0; i < glyphImageData.size() / 4; i++)
				glyphImageData[(i * 4) + 3] = pGlyph->m_pImageData[i];

			glTexSubImage2D(GL_TEXTURE_2D,
							0,
							pGlyph->GetSourceX(),
							pGlyph->GetSourceY(),
							pGlyph->GetWidth(),
							pGlyph->GetHeight(),
							GL_RGBA,
							//GL_RED,
							GL_UNSIGNED_BYTE,
							glyphImageData.data());
							//pGlyph->m_pImageData);
			delete [] pGlyph->m_pImageData;
			pGlyph->m_pImageData = NULL;
		}
	}

	// Finalize the texture params.
	TextureParams texParams;
	texParams.SetTarget(TextureTarget::TEXTURE_2D);
	texParams.SetWrap(TextureWrap::CLAMP_TO_BORDER);
	texParams.SetFiltering(TextureFilter::TRILINEAR);
	m_pGlyphAtlasTexture->SetParams(texParams);
	m_pGlyphAtlasTexture->GenerateMipMaps();
}

