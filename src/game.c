#include <stdbool.h>
#include "ui.h"
#include "generation.h"
#include "game.h"
#include <ncurses.h>
#include "player.h"
#include "gameloop.h"
#include <stdlib.h>

MENU_OPTION menu_option = NEW_GAME;
CHARACTER_MENU character_option = NAME;
STATE current_state = MENU;
bool running = true;

void menuloop(){
	while(running){
		switch(current_state){
			case MENU:
				menu();
				break;
			case CHARACTER:
				initializeCharacterMenu();
				createCharacter();
				break;
			case LOAD_MENU:
				loadMenu();
				break;
		}

	}
};

void menu(){
	int ch;
	drawMenu(menu_option);
	ch = getch();
	switch(ch){
		case 'e':
		case '\n':
			menuSelect(menu_option);
			break;
		case 's':
			menu_option = menu_option==2?0:menu_option + 1;
			break;
		case 'w':
			menu_option = menu_option==0?2:menu_option - 1;
			break;
		case 'q':
			quit();
			break;
		default:
			break;
	}
};

void quit(){
	running=false;
}

void menuSelect(MENU_OPTION option){

	switch(option){
		case 0:
			current_state = CHARACTER;
			break;
		case 1:
			current_state = LOAD_MENU;
			break;
		case 2:
			running=false;
			break;
	}
}

void createCharacter(){
	character_option = 0;
	while(current_state == CHARACTER){
		int ch;
		drawCharacterMenu(character_option);
		ch = getch();
		switch(ch){
			case 'e':
			case '\n':
				characterMenuSelect(character_option);
				break;
			case 's':
				character_option = character_option==8?0:character_option+1;
				break;
			case 'w':
				character_option = character_option==0?8:character_option-1;
				break;
			case 'a':
				if(character_option > 0 && character_option < 7){
					removeSkillPoint(character_option);
				}
				break;
			case 'd':
				if(character_option > 0 && character_option < 7){
					addSkillPoint(character_option);
				}
				break;
			case 'q':
				current_state = MENU;
				break;

		}
	}
}

void characterMenuSelect(CHARACTER_MENU option){
	switch(option){
		case NAME:
			nameFocus();
			break;
		case CONFIRM:
			characterConfirmation();
			gameloop();
			current_state = MENU;
			break;
		case CANCEL:
			current_state = MENU;
			break;
	}
}

void loadMenu(){
	current_player = calloc(1, sizeof(Player));
	FILE * file = fopen("saves/main.sav", "rb");
	if(file != NULL){
		char buffer[sizeof(Player)];
		fread(buffer, sizeof(Player), 1, file);
		memcpy(current_player, buffer, sizeof(Player));
		current_player->floor--;
		fclose(file);
	}
	move(0, 0);
	gameloop();
	current_state = MENU;
}

void saveGame(){
	char buffer[sizeof(Player)];
	memcpy(buffer, current_player, sizeof(Player));
	FILE* file = fopen("saves/main.sav", "wb");
	if(file != NULL){
		fwrite(buffer, sizeof(Player), 1, file);
		fclose(file);
	}
}
