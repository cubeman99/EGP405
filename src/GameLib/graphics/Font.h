#ifndef _FONT_H_
#define _FONT_H_

#include <ft2build.h>
#include FT_FREETYPE_H
#include <math/Vector2f.h>
#include <graphics/Texture.h>
#include <util/ShelfBinPacker.h>
#include <vector>
#include <map>

/*
       (-y)          =================
         .             GLYPH METRICS 
        /|\          =================
         |   minX
	     |    .          .
	     |    .   width  .
	     |    <---------->
	     |    .          .
         |    +----------+ . . . . . .
         |    |  .--./)  |           |
         |    | /.''\\   |           |
         |    | |  | |   |           |
         |    | \`-' /   |           |
         |    | /("''    |           |
(-x) <---@----+-\-'---.--+-----@-->  | height
         |    |  /'""'.\ |     .     |
         |    | ||     |||     .     | 
         |    | \'. __// |     .     |
         |    |  `'---'  |     .     |
         |    +----------+ . . . . . . . . minY
	     |                     . 
	     |                     . 
	     |-------------------->.
	     |        advance                 
        \|/
         '
       (+y)
*/

struct Point2i
{
	int x;
	int y;

	Point2i() {}
	Point2i(int x, int y) : x(x), y(y) {}
};

//-----------------------------------------------------------------------------
// Glyph
//-----------------------------------------------------------------------------

// A Glyph represents the metrics and image
// offset for a single character code.
class Glyph
{
	friend class Font;

public:
	Glyph() :
		m_pImageData(NULL)
	{}

	// Accessors.
	inline bool HasImage()		const { return (m_width > 0 && m_height > 0); }
	inline int GetAdvance()		const { return m_advance; }
	inline int GetMinX()		const { return m_minX; }
	inline int GetMinY()		const { return m_minY; }
	inline int GetMaxX()		const { return m_minX + m_width; }
	inline int GetMaxY()		const { return m_minY + m_height; }
	inline int GetWidth()		const { return m_width; }
	inline int GetHeight()		const { return m_height; }
	inline int GetSourceX()		const { return m_sourceX; }
	inline int GetSourceY()		const { return m_sourceY; }
	inline int GetAtlasIndex()	const { return m_atlasIndex; }
	inline unsigned long GetCharCode()	const { return m_charCode; }

private:
	Glyph(FT_GlyphSlot pFTGlyph, unsigned long charCode);

	int m_advance;		// Amount to move the pen position after drawing this glyph.
	int m_minX;			// Image x-draw-offset from pen position.
	int m_minY;			// Image y-draw-offset from pen position.
	int m_width;		// Width of glyph image.
	int m_height;		// Height of glyph image.
	int m_sourceX;		// Image X-offset in the font's glyph atlas texture.
	int m_sourceY;		// Image Y-offset in the font's glyph atlas texture.
	int m_atlasIndex;	// The index of the glyph atlas texture to use.

	unsigned char*	m_pImageData;
	unsigned long	m_charCode;		// The unicode value for this character.
};





//-----------------------------------------------------------------------------
// Font
//-----------------------------------------------------------------------------

struct CharacterRegion
{
	unsigned long begin;	// First character code in the region.
	unsigned long length;	// Number of characters in the region.
};

// A fixed-size bitmap font that stores glyph images
// for a region of character codes.
class Font
{
	typedef std::map<unsigned long, Glyph*> GlyphMap;

public:
	~Font();
	
	// Accessors
	Glyph						GetGlyph(char c) const;
	Glyph						GetGlyph(unsigned long charCode) const;
	bool						IsCharDefined(unsigned long charCode) const;
	inline int					GetSize()			const { return m_size; }
	inline const std::string&	GetFamilyName()		const { return m_familyName; }
	inline const std::string&	GetStyleName()		const { return m_styleName; }
	inline const Texture*		GetGlyphTexture()	const { return m_pGlyphAtlasTexture; }
	inline Texture*				GetGlyphTexture()		  { return m_pGlyphAtlasTexture; }
	inline const Point2i&		GetPixelsPerEM()		  { return m_pixelsPerEM; }

	// FreeType library
	static bool  InitFreeTypeLibrary();
	static void  QuitFreeTypeLibrary();

	// Font loading.
	static Font* LoadFont(const std::string& filename, int size);
	static Font* LoadFont(const std::string& filename, int size, unsigned long charRegionBegin, unsigned long charRegionEnd);
	static Font* LoadDistanceFieldFont(const std::string& filename, int size,
			unsigned long charRegionBegin, unsigned long charRegionEnd, int scale, float spread);
	
	void LoadGlyph(unsigned long charCode);
	void LoadGlyphRange(unsigned long rangeStart, unsigned long rangeEnd);
	void CreateGlyphAtlas();

private:
	Font(FT_Face pFace, int size);
	Font(FT_Face pFace, int size, unsigned long charRegionBegin, unsigned long charRegionEnd, int scale, float spread);
	
	static FT_Library msFreeTypeLibrary;

	std::string		m_familyName;
	std::string		m_styleName;
	int				m_size;
	Texture*		m_pGlyphAtlasTexture;
	GlyphMap		m_glyphMap;

	// Font Creation.
	ShelfBinPacker	m_glyphPacker;
	FT_Face			m_ftFace;

	Point2i			m_pixelsPerEM; // The size of the EM square in pixels.
};


#endif // _FONT_H_