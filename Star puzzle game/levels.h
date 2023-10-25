#pragma once
#ifndef LEVELS_H
#define LEVELS_H
#include "console_game.h"

#define MAX_TELEPORTERS 128

struct Teleporter
{
	POINT a;
	POINT b;
};

struct level
{
	wchar_t** level;
	wchar_t* level_name;
	size_t level_size;
	int spawn_x;
	int spawn_y;
	int num_stars;
	int level_id;
	struct Teleporter teleporters[MAX_TELEPORTERS];
};

struct level* load_level(int level);
void destroy_current_level();
struct level* current_level();
BOOL position_in_bounds(struct level* pLevel, int player_x, int player_y);
wchar_t get_level_geometry_at(struct level* pLevel, int x, int y);
BOOL is_star(struct level* pLevel, int x, int y);
void display_level(ConsoleGame_t* game, struct level* pLevel);
void CheckAndCollectStar(ConsoleGame_t* game, struct level* pLevel, float* pplayer_x, float* pplayer_y, int* score, enum move_direction* direction);
void AttemptTeleport(ConsoleGame_t* game, struct level* pLevel, float* player_x, float* player_y, enum move_direction direction);

#endif // LEVELS_H