#ifndef _BOARD_H_
#define _BOARD_H_

#define BOARD_WIDTH 3
#define BOARD_HEIGHT 3


struct Board
{
public:
	Board();

	void Clear();

	void Display();

	bool IsSpaceEmpty(int index) const;
	char GetSpace(int index) const;
	void SetSpace(int index, char c);

	int GetWidth() const;
	int GetHeight() const;

	bool CheckWin(char& winner, bool& isDraw);


private:
	char m_grid[BOARD_WIDTH * BOARD_HEIGHT];
};


#endif // _BOARD_H_