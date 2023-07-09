#ifndef PLAYER_H
#define PLAYER_H

#include "generation.h"
#include "world.h"
#include <string.h>


Player *createPlayer();
int getDamage(Weapon * weapon, Player * player);
int getSpellDamage(Spell * spell, Player * player);

#endif
