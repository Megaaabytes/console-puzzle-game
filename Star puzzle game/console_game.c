#include "console_game.h"
#include <stdlib.h>
#include <stdio.h>

#define DEFAULT_FORE_COLOUR FOREGROUND_INTENSITY

WORD GetColour(ConsoleGame_t* game)
{
	if (!game)
		return DEFAULT_FORE_COLOUR;

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (!GetConsoleScreenBufferInfo(game->screen_handle, &csbi))
		return DEFAULT_FORE_COLOUR;

	return csbi.wAttributes;
}

BOOL SetColour(ConsoleGame_t* game, unsigned short colour)
{
	if (!game)
		return FALSE;

	if (!SetConsoleTextAttribute(game->screen_handle, colour))
		return FALSE;

	return TRUE;
}

_Success_(return == TRUE)
BOOL GetConsoleColumnsAndRows(_In_ HANDLE consoleHandle, _Out_ int* columns, _Out_ int* rows)
{
	if (!columns || !rows)
		return FALSE;

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (!GetConsoleScreenBufferInfo(consoleHandle, &csbi))
		return FALSE;

	*columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	*rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	return TRUE;
}

ConsoleGame_t InitGame()
{
	ConsoleGame_t game = { 0 };

	// Create the screen buffer.
	game.screen_handle = CreateConsoleScreenBuffer(
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		CONSOLE_TEXTMODE_BUFFER,
		NULL
	);

	// Get the console's dimensions.
	if (GetConsoleColumnsAndRows(game.screen_handle, &game.columns, &game.rows) == FALSE) {
		fwprintf(stderr, L"Failed to retrieve console columns and rows.\n");
		exit(EXIT_FAILURE);
	}

	/* Get the maximum size for the console window. */
	/* We can't get a larger console buffer, so reallocations are not necessary. */
	game.largestBufferSize = GetLargestConsoleWindowSize(game.screen_handle);

	// Allocate frame buffer;
	game.frame_buffer = calloc((int) game.largestBufferSize.X * (int) game.largestBufferSize.Y, sizeof(wchar_t));
	if (game.frame_buffer == NULL) {
		fwprintf(stderr, L"Not enough memory.\n");
		exit(EXIT_FAILURE);
	}

	/* If we can't change the screen buffer, we don't have a game. */
	if (!SetConsoleActiveScreenBuffer(game.screen_handle)) {
		fwprintf(stderr, L"Failed to change console screen buffer.");
		DestroyGame(&game);
		exit(EXIT_FAILURE);
	}

	return game;
}

_Success_(return == TRUE)
BOOL CheckConsoleWindowBounds(ConsoleGame_t* game)
{
	if (!game)
		return FALSE;

	int columns, rows;
	GetConsoleColumnsAndRows(game->screen_handle, &columns, &rows);
	
	if (game->columns != columns || game->rows != rows) {
		game->columns = columns;
		game->rows = rows;
	}

	return TRUE;
}

BOOL ClearScreen(ConsoleGame_t* game)
{
	if (!game)
		return FALSE;

	memset(game->frame_buffer, 0, (game->rows * game->columns) * sizeof(wchar_t));
	return TRUE;
}

BOOL SwapFrame(ConsoleGame_t* game)
{
	if (!game)
		return FALSE;

	/* Got lazy, didn't like if statement. */
	DWORD nLength = (game->rows * game->columns) > (game->largestBufferSize.X * game->largestBufferSize.Y) ? (game->largestBufferSize.X * game->largestBufferSize.Y) : (game->rows * game->columns);
	DWORD dwWritten;
	if (!WriteConsoleOutputCharacterW(game->screen_handle, game->frame_buffer, nLength, (COORD) { 0, 0 }, &dwWritten)) // Draw the screen buffer.
		return FALSE;

	return TRUE;
}

void DestroyGame(ConsoleGame_t* game)
{
	CloseHandle(game->screen_handle);
	free(game->frame_buffer);
}

BOOL PutPixel(ConsoleGame_t* game, wchar_t pixel, int x, int y)
{
	if (!game)
		return FALSE;

	/* Technically no error, its just out of view of the camera. */
	if (x > game->columns || y > game->rows || x < 0 || y < 0)
		return TRUE;

	game->frame_buffer[y * game->columns + x] = pixel;
	return TRUE;
}

BOOL PutColouredPixel(ConsoleGame_t* game, wchar_t pixel, int x, int y, WORD colour)
{
	if (!game)
		return FALSE;

	unsigned short oldColour = GetColour(game);
	SetColour(game, colour);
	if (!PutPixel(game, pixel, x, y))
		return FALSE;
	
	SetColour(game, oldColour);
	return TRUE;
}

BOOL PutString(ConsoleGame_t* game, wchar_t* str, size_t len, int x, int y)
{
	if (!game || !str)
		return FALSE;

	if (len == 0)
		return TRUE;

	for (size_t i = 0; i < len; ++i) {
		PutPixel(game, str[i], x + (int) i, y);
	}

	return TRUE;
}

BOOL PutColouredString(ConsoleGame_t* game, wchar_t* str, size_t len, int x, int y, WORD colour)
{
	if (!game)
		return FALSE;

	unsigned short oldColour = GetColour(game);
	SetColour(game, colour);
	if (!PutString(game, str, len, x, y))
		return FALSE;

	SetColour(game, oldColour);
	return TRUE;
}
