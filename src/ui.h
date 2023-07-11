#ifndef UI_H
#define UI_H

#include "player.h"
#include <stdbool.h>

void drawMenu(int selected);
void handleMenuInput(char ch);
void drawStats();
void drawEnemy(char name[]);
void drawBattle();
void drawInventory();
void drawSpellbook(int selected);
void drawDeath();
void drawCharacterMenu();
void addSkillPoint(int skill);
void removeSkillPoint(int skill);
void nameFocus();
void initializeCharacterMenu();
void characterConfirmation();
void equipOffHand(int index);
void inventorySelect(int sub_section, int index);
extern Player *current_player;
extern bool action_taken;
extern char action_message[];
extern bool manual_attack;
extern bool is_scroll;
extern bool is_battling;
extern Spell spell_from_scroll;

#endif
