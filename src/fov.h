#ifndef FOV_H
#define FOV_H

#include "player.h"
#include "generation.h"

void makeFOV(Player* player);
void clearFOV(Player* player);
int getDistance(Position origin, Position target);
bool isInMap(int y, int x);
bool lineOfSight(Position origin, Position target);
int getSign(int a);

#endif
