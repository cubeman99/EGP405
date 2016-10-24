#include "board.h"
#include <stdio.h>
#include <string.h>
#include <Windows.h>


#define NUM_WIN_POSSIBILITIES 8

// 0 1 2
// 3 4 5
// 6 7 8

const int WIN_INDICES[NUM_WIN_POSSIBILITIES][3] {
	{ 0, 1, 2 },
	{ 3, 4, 5 },
	{ 6, 7, 8 },

	{ 0, 3, 6 },
	{ 1, 4, 7 },
	{ 2, 5, 8 },

	{ 0, 4, 8 },
	{ 2, 4, 6 },
};


// Get the printable character for a board-space character.
static char GetSpaceChar(char c)
{
	if (c == 0)
		return ' ';
	return c;
}


Board::Board()
{
	Clear();
}

void Board::Clear()
{
	memset(m_grid, 0, sizeof(m_grid));
}

void Board::Display()
{
	printf("   %c | %c | %c      1 | 2 | 3 \n", GetSpaceChar(m_grid[0]), GetSpaceChar(m_grid[1]), GetSpaceChar(m_grid[2]));
	printf("  ---+---+---    ---+---+---\n");
	printf("   %c | %c | %c      4 | 5 | 6 \n", GetSpaceChar(m_grid[3]), GetSpaceChar(m_grid[4]), GetSpaceChar(m_grid[5]));
	printf("  ---+---+---    ---+---+---\n");
	printf("   %c | %c | %c      7 | 8 | 9 \n", GetSpaceChar(m_grid[6]), GetSpaceChar(m_grid[7]), GetSpaceChar(m_grid[8]));
}

bool Board::IsSpaceEmpty(int index) const
{
	return (m_grid[index] == 0);
}

char Board::GetSpace(int index) const
{
	return m_grid[index];
}

void Board::SetSpace(int index, char c)
{
	m_grid[index] = c;
}

int Board::GetWidth() const
{
	return BOARD_WIDTH;
}

int Board::GetHeight() const
{
	return BOARD_HEIGHT;
}

bool Board::CheckWin(char& winner, bool& isDraw)
{
	// Check each winning board state.
	for (int i = 0; i < NUM_WIN_POSSIBILITIES; i++)
	{
		// Check if one symbol is in the 3 contiguous spaces.

		winner = m_grid[WIN_INDICES[i][0]];

		if (winner != 0)
		{
			for (int j = 1; j < 3; j++)
			{
				char c = m_grid[WIN_INDICES[i][j]];

				if (c != winner)
				{
					winner = 0;
					break;
				}
			}

			if (winner != 0)
			{
				// We found a winner!
				isDraw = false;
				return true; // The game is ended.
			}
		}
	}

	// There is no winner, so now check for a draw.
	for (int i = 0; i < BOARD_WIDTH * BOARD_HEIGHT; i++)
	{
		if (m_grid[i] == 0)
		{
			winner = 0;
			isDraw = false;
			return false; // The game has NOT ended.
		}
	}

	winner = 0;
	isDraw = true;
	return true; // The game is ended in a tie.
}