#ifndef GENERATION_H
#define GENERATION_H

#include <stdbool.h>
#include "world.h"

#define VISIBLE_COLOR 1
#define SEEN_COLOR 2

const static int MAP_HEIGHT = 40;
const static int MAP_WIDTH = 100;

typedef struct {
	int ch;
	int color;
	Position pos;
	bool walkable;
	bool transparent;
	bool visible;
	bool seen;
	bool has_item;
	bool has_monster;
	bool has_transition;
	Monster * monster;
	Item * item;
} Tile;

typedef struct {
	int height;
	int width;
	Position pos;
	Position center;
} Room;

Item * generateWeapon();
Item * generateArmor();
Item * generateSpell();
Item * generateConsumable();
Position generateMap();
void regenerateMap(bool has_moved);
void drawMap();
Room createRoom(int y, int x, int height, int width);
void addRoom(Room room);
Position setupRooms();
Monster selectBoss();
void generateBossFloor();
void bossBeaten();
void moveMonsters();
void connectRooms();
void addMonsters();
Tile** createMapTiles();
extern Tile**map;

Monster selectMonster(int x, int y);
Item * generateItem();

#endif
