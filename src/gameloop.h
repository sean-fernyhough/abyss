

void gameloop();
void handleInput();
void pickupItem(Item * item, int y, int x);
void monsterMovement();
void death();
void playerMove();
typedef enum {
	MAIN,
	INVENTORY,
	BATTLE
} GAME_STATE;
