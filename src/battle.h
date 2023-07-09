#include "player.h"

void drawBattle();
void battleLoop(Monster * a, Player * b);

typedef enum {
	ATTACK,
	CAST,
	DEFEND,
	FLEE
} BATTLE_STATE;

void battleMenuSelect();
void attack();
void displaySpells();
void cast(Spell spell);
void defend();
void flee();
void battleMessage(char message[]);
void drawMonster();
void monsterAttack();
void drawStats();
