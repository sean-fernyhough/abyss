#ifndef GAME_H
#define GAME_H

#include "player.h"
#include "generation.h"

typedef enum {
	MENU,
	CHARACTER,
	LOAD_MENU,
} STATE;

typedef enum {
	NEW_GAME,
	LOAD_GAME,
	EXIT
} MENU_OPTION;

typedef enum {
	NAME,
	CONSTITUTION,
	STRENGTH,
	DEXTERITY,
	INTELLIGENCE,
	ENDURENCE,
	WISDOM,
	CONFIRM,
	CANCEL
} CHARACTER_MENU;

void menu();
void menuSelect(MENU_OPTION menu_option);
void battle();
void inventory();
void quit();
void createCharacter();
void loadMenu();
void loadGame();
void saveGame();
void menuloop();
void displaMessage(char message[]);
void characterMenuSelect(CHARACTER_MENU character_option);

typedef struct {
	Player *player;
	int level;
	Tile**map;
} Save_Data;

#endif
