#include <stdbool.h>
#include "generation.h"
#include "player.h"
#include "ui.h"
#include <ncurses.h>
#include "fov.h"
#include "gameloop.h"
#include "battle.h"
#include "game.h"

bool main_game_running;
bool has_moved = false;
bool is_dead;
unsigned int steps = 0;
unsigned int mana_step = 0;

void gameloop(){
	int ch;
	clear();
	main_game_running = true;
	current_player->pos = generateMap();
	mvaddch(current_player->pos.y, current_player->pos.x, current_player->ch);

	while(main_game_running){
		clear();
		if(has_moved){
			moveMonsters();
			if(is_dead){
			clear();
			is_dead = false;
			return;
		}
		}
		clearFOV(current_player);
		makeFOV(current_player);
		regenerateMap(has_moved);
		mvaddch(current_player->pos.y, current_player->pos.x, current_player->ch);
		has_moved = false;
		handleInput();
	}
}

void handleInput(){
	int ch;
	ch = getch();
	switch(ch){
		case 'w':
			if(map[current_player->pos.y-1][current_player->pos.x].walkable){
				current_player->pos.y--;
				playerMove();
			}
			break;
		case 's':
			if(map[current_player->pos.y+1][current_player->pos.x].walkable){
				current_player->pos.y++;
				playerMove();
			}
			break;
		case 'a':
			if(map[current_player->pos.y][current_player->pos.x-1].walkable){
				current_player->pos.x--;
				playerMove();
			}
			break;
		case 'd':
			if(map[current_player->pos.y][current_player->pos.x+1].walkable){
				current_player->pos.x++;
				playerMove();
			}
			break;
		case 'e':
			drawInventory();
			break;
		case 'q':
			main_game_running = false;
			break;
	}
//			if(current_player->pos.x == monsters[0].pos.x && current_player->pos.y == monsters[0].pos.y){
//		}
}

void playerMove(){
	steps++;
	if(current_player->health < current_player->max_health){
		//current_player->health++;
	}
	if(current_player->mana < current_player->max_mana && steps >= (mana_step+2)){
		current_player->mana++;
		mana_step = steps;
	}
	if(map[current_player->pos.y][current_player->pos.x].has_item){
		pickupItem(map[current_player->pos.y][current_player->pos.x].item, current_player->pos.y, current_player->pos.x);
	}else if(map[current_player->pos.y][current_player->pos.x].has_monster){
		if(!map[current_player->pos.y][current_player->pos.x].monster->dead || map[current_player->pos.y][current_player->pos.x].monster->wait_time == 0){
			battleLoop(map[current_player->pos.y][current_player->pos.x].monster, current_player);
		}
	}else if(map[current_player->pos.y][current_player->pos.x].has_transition){
		clear();
		current_player->pos = generateMap();
		saveGame();
	}
	has_moved = true;
}

void pickupItem(Item * item, int y, int x){
	if(item->type == SPELL){
		int spell_index = -1;
		for(int i = 0; i < (sizeof(current_player->spellbook)/sizeof(Spell)); i++){
			if((current_player->spellbook[i].name[0]) == '\0'){
				spell_index = i;
				break;
			}
		}
		if(spell_index != -1){
			mvvline(40, 0, '#', 7);
			mvvline(40, 119, '#', 7);
			mvhline(40, 0, '#', 120);
			mvhline(46, 0, '#', 120);
			current_player->spellbook[spell_index] = item->spell;
			move(43, 2);
			map[current_player->pos.y][current_player->pos.x].has_item = false;
			map[current_player->pos.y][current_player->pos.x].ch = '.';
			printw("You found a %s tome!", current_player->spellbook[spell_index].name);
			getch();
		}else{
			mvvline(40, 0, '#', 7);
			mvvline(40, 119, '#', 7);
			mvhline(40, 0, '#', 120);
			mvhline(46, 0, '#', 120);
			move(43, 2);
			printw("You cannot learn any more spells!");
			getch();

		}
	}else{
		int inv_index = -1;
	for(int i = 0; i < (sizeof(current_player->inventory)/sizeof(Item)); i++){
		if((current_player->inventory[i].name[0]) == '\0'){
		 inv_index = i;
		 break;
		}
	}
	if(inv_index != -1){
			mvvline(40, 0, '#', 7);
			mvvline(40, 119, '#', 7);
			mvhline(40, 0, '#', 120);
			mvhline(46, 0, '#', 120);
			current_player->inventory[inv_index] = *item;
			move(43, 2);
			map[y][x].has_item = false;
			map[y][x].ch = '.';
			printw("You found a %s", current_player->inventory[inv_index].name);
			getch();
		}else{
			mvvline(40, 0, '#', 7);
			mvvline(40, 119, '#', 7);
			mvhline(40, 0, '#', 120);
			mvhline(46, 0, '#', 120);
			move(43, 2);
			printw("You have no room in your inventory");
			getch();
		}
	}
}


void death(){
	clear();
	main_game_running = false;
	is_dead = true;
	FILE *mptr = fopen("src/grave.txt", "r");
	int fch;
	int col = 0;
	int row = 0;
	while((fch = fgetc(mptr))!=EOF){
		if(fch == '\n'){
			row++;
			col = 0;
		}else{
			mvaddch(row, col, fch);
			col++;
		}
	}
	move(15, 50);
	printw("Name: %s", current_player->name);
	mvaddch(15, 33, '/');
	mvaddch(15, 85, '\\');
	mvaddch(17, 32, '|');
	mvaddch(17, 86, '|');
	move(17, 50);
	printw("Level: %d", current_player->lvl);
	move(19, 50);
	printw("Floor Reached: %d", current_player->floor);
	mvaddch(19, 32, '|');
	mvaddch(19, 86, '|');
	getch();
}
