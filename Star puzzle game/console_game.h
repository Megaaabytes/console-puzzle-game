#pragma once
#define WIN32_LEAN_AND_MEAN
#define STRICT

#include <Windows.h>

#define PLAYER (wchar_t) 0x263a

enum move_direction
{
	DORMANT,
	LEFT,
	RIGHT,
	UP,
	DOWN,
};

typedef struct ConsoleGame
{
	HANDLE screen_handle;
	wchar_t* frame_buffer;
	int columns;
	int rows;
	COORD largestBufferSize;
} ConsoleGame_t;

WORD GetColour(ConsoleGame_t* game);
BOOL SetColour(ConsoleGame_t* game, unsigned short colour);

_Success_(return == TRUE)
BOOL GetConsoleColumnsAndRows(_In_ HANDLE consoleHandle, _Out_ int* columns, _Out_ int* rows);

ConsoleGame_t InitGame();

_Success_(return == TRUE)
BOOL CheckConsoleWindowBounds(ConsoleGame_t* game);

BOOL ClearScreen(ConsoleGame_t* game);
BOOL SwapFrame(ConsoleGame_t* game);
void DestroyGame(ConsoleGame_t* game);

BOOL PutPixel(ConsoleGame_t* game, wchar_t pixel, int x, int y);
BOOL PutColouredPixel(ConsoleGame_t* game, wchar_t pixel, int x, int y, WORD colour);
BOOL PutString(ConsoleGame_t* game, wchar_t* str, size_t len, int x, int y);
BOOL PutColouredString(ConsoleGame_t* game, wchar_t* str, size_t len, int x, int y, WORD colour);
