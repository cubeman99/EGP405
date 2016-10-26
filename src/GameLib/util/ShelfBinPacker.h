#ifndef _SHELF_BIN_PACKER_H_
#define _SHELF_BIN_PACKER_H_

#include <vector>

/*
   SHELF BIN-PACKING ALGORITHM

 ##############################
 #......#.....#...#....#....# #
 #......#.....#...#....#....# #
 #......#.....#...#....#....# #
 #......#.....#...#....###### #
 #......#.....#...#....#      #
 #......#.....##########      #
 #......#######               #
 #......#                     #
 ##############################
 #.....#...#.....#....#...#...#
 #.....#...#.....#....#########
 #.....#...############       #
 #.....#...#                  #
 ##############################

*/

// This class is used to pack rectangles into a bin, using
// the shelf packing algorithm. This is helpful for
// packing sprites into a sprite sheet, and is currently
// being used to pack several glyph images for a font
// into a single texture.
class ShelfBinPacker
{
public:
	ShelfBinPacker();

	void Insert(int* x, int* y, int width, int height, int padding = 0);
	void Pack(int* binWidth, int* binHeight);
	void Clear();

private:
	struct BinRect
	{
		int* pX;
		int* pY;
		int  width;
		int  height;
		int  padding;
	};

	std::vector<BinRect> m_rects;
	int m_rectAreaSum;
};


#endif // _SHELF_BIN_PACKER_H_