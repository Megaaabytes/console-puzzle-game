#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>
#include "console_game.h"
#include "levels.h"

#define PLAYER_SPEED 0.13f

#define ARROW_RIGHT_KEY 0x4d
#define ARROW_LEFT_KEY  0x4b
#define ARROW_DOWN_KEY  0x50
#define ARROW_UP_KEY    0x48

static BOOL exit_flag = FALSE;

void player_move(ConsoleGame_t* game, struct level* pLevel, float* player_x, float* player_y, int* score)
{
	if (!game || !pLevel || !player_x || !player_y || !score)
		return;

	/* Which way the player is moving, if DORMANT, the player is not moving. */
	static enum move_direction direction = DORMANT;
	
	AttemptTeleport(game, current_level(), player_x, player_y, direction); /* If the player is on a teleporter pixel, teleport them. */
	if (direction == DORMANT) {
		int key = _getch(); // We use _getch() instead of GetAsyncKeyState(), because _getch() makes the thread sleep rather than wasting cpu cycles.

		switch (key) {
		case ARROW_RIGHT_KEY:
			/* We don't want to clip outside the level. */
			if (position_in_bounds(current_level(), ((int)*player_x) + 1, (int)(*player_y))) {
				++(*player_x);
				direction = RIGHT;
			}

			break;
		case ARROW_LEFT_KEY:
			if (position_in_bounds(current_level(), ((int)*player_x) - 1, (int)(*player_y))) {
 				--(*player_x);
 				direction = LEFT;
			}

			break;
		case ARROW_DOWN_KEY:
			if (position_in_bounds(current_level(), (int)(*player_x), ((int)*player_y) + 1)) {
				++(*player_y);
				direction = DOWN;
			}

			break;
		case ARROW_UP_KEY:
			if (position_in_bounds(current_level(), (int)*player_x, ((int)*player_y) - 1)) {
				--(*player_y); 
				direction = UP;
			}

			break;
		case 'r':
			if (exit_flag) {
				load_level(pLevel->level_id);
				*player_x = (float)current_level()->spawn_x;
				*player_y = (float)current_level()->spawn_y;
				*score = 0;
			}

			exit_flag = !exit_flag;
			return;
		}

		exit_flag = FALSE;
	}
	else {
		/* Keep moving until we reach a wall. */
		switch (direction) {
		case LEFT:
			if (get_level_geometry_at(current_level(), ((int) *player_x) - 1, (int) *player_y) == L'#') {
				direction = DORMANT;
				break;
			}
			
			(*player_x) -= PLAYER_SPEED;
			break;
		case RIGHT:
			if (get_level_geometry_at(current_level(), ((int) *player_x) + 1, (int) *player_y) == L'#') {
				direction = DORMANT;
				break;
			}

			(*player_x) += PLAYER_SPEED;
			break;
		case DOWN:
			if (get_level_geometry_at(current_level(), (int) *player_x, ((int) *player_y) + 1) == L'#') {
				direction = DORMANT;
				break;
			}

			(*player_y) += PLAYER_SPEED;
			break;
		case UP:
			if (get_level_geometry_at(current_level(), (int) *player_x, ((int) *player_y) - 1) == L'#') {
				direction = DORMANT;
				break;
			}

			(*player_y) -= PLAYER_SPEED;
			break;
		}
	}

	/* If the player hit a star, collect it and possibly end the level. */
	CheckAndCollectStar(game, current_level(), player_x, player_y, score, &direction);
}

static wchar_t warning_msg[] = L"Press R again to reload the current level.";
const static size_t warn_msg_len = ARRAYSIZE(warning_msg) - 1;

void main_game_loop(ConsoleGame_t* game)
{
	load_level(0); /* Load the first level. */
	float player_x = (float) current_level()->spawn_x; /* Set the players starting positions to the levels starting positions. */
	float player_y = (float) current_level()->spawn_y; /* Set the players starting positions to the levels starting positions. */
	int score = 0;
	
	while (TRUE) {
		ClearScreen(game); // Before drawing, we clear the buffer.
		CheckConsoleWindowBounds(game); // We also want to update the bounds of the frame buffer.
		display_level(game, current_level()); // Then display it.
		
		/* This is to warn the player they pressed the restart level button. */
		if (exit_flag) {
			PutString(game, warning_msg, warn_msg_len, game->columns - (int)warn_msg_len - 65, 0);
		}
		
		PutString(game, current_level()->level_name, lstrlenW(current_level()->level_name), 2, 0); /* Put the level name on the screen for aesthetics. */
		PutPixel(game, PLAYER, (int) player_x, (int) player_y); /* Put the player pixel, otherwise the player wouldn't know where they are ;). */
		SwapFrame(game); /* Display the frame to the player. */
		player_move(game, current_level(), &player_x, &player_y, &score); /* Get input from the user and move the player accordingly. */
	}
}

int main()
{
	ConsoleGame_t game = InitGame();
	main_game_loop(&game);
	DestroyGame(&game);
	destroy_current_level();
	return 0;
}