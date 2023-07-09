#include <stdbool.h>
#include <ncurses.h>
#include "generation.h"
#include "player.h"
#include "battle.h"
#include <stdlib.h>
#include "world.h"
#include "gameloop.h"
#include <math.h>
#include "ui.h"

Player * player;
Monster * monster;
BATTLE_STATE battle_state = ATTACK;
bool is_defending = false;
int battle_option = 0;
int spell_selection = 0;
int bch;
bool timer_should_progress = false;

void battleLoop(Monster * a, Player * b){
	battle_option = 0;
	spell_selection = 0;
	is_battling = true;
	monster = a;
	player = b;
	char encounter_message[100];
	sprintf(encounter_message, "Encountered a %s!", monster->name);
	battleMessage(encounter_message);
	while(is_battling){
		drawBattle();
		drawStats();
		if(monster->status != NO_STATUS && (timer_should_progress || action_taken)){
			int health1 = monster->health;
			switch(monster->status){
				case POISONED:
					monster->health -= 2;
					break;
				case BLEEDING:
					monster->health -= 6;
					break;
				case BURNING:
					monster->health -= 4;
					break;
			}
			int health2 = monster->health;
			if(monster->health <= 0){
				char dam_message[255];
				sprintf(dam_message, "%s took %d damage!", monster->name, health1 - health2);
				battleMessage(dam_message);
				sprintf(dam_message, "You have killed %s!", monster->name);
				battleMessage(dam_message);
				map[monster->pos.y][monster->pos.x].has_monster = false;
				map[monster->pos.y][monster->pos.x].ch = '.';
				player->status = NO_STATUS;
				monster->dead = true;
				int reward_exp = monster->exp;
				while(reward_exp > 0){
					if((player->exp + reward_exp) >= (player->lvl * 3)){
						reward_exp -= ((player->lvl * 3)-player->exp);
						player->exp = 0;
						player->skill_points++;
						player->lvl++;
						player->health = player->max_health;
						player->mana = player->max_mana;
						battleMessage("You have leveled up!");
					}else{
						player->exp += reward_exp;
						reward_exp = 0;
					}
				}
				clear();
				is_battling = false;
				return;
			}else{
				if(monster->status == BURNING || monster->status == BLEEDING || monster->status == POISONED){
					char dam_message[255];
					sprintf(dam_message, "%s took %d damage!", monster->name, health1 - health2);
					battleMessage(dam_message);
				}
				if(monster->status_timer == 0){
					monster->status = NO_STATUS;
					char dam_message[255];
					sprintf(dam_message, "%s has recovered!", monster->name);
					battleMessage(dam_message);
				}else{
					monster->status_timer--;
				}
			}
		}
		//finish status effects
		if(player->status != NO_STATUS && (timer_should_progress || action_taken)){
			int health1 = player->health;
			switch(player->status){
				case POISONED:
					player->health -= 2;
					break;
				case BLEEDING:
					player->health -= 6;
					break;
				case BURNING:
					player->health -= 4;
					break;
			}
			int health2 = player->health;
			if(player->health <= 0){
				battleMessage("You died...");
				is_battling = false;
				death();
			}else{
				player->status_timer--;
				char dam_message[255];
				sprintf(dam_message, "You took %d damage!", health1 - health2);
				battleMessage(dam_message);
				if(player->status_timer == 0){
					player->status = NO_STATUS;
				}else{
					player->status_timer--;
				}
			}
		}
		drawBattle();
		drawStats();
		is_defending = false;
		timer_should_progress = false;
		if(action_taken){
			action_taken = false;
			if(is_scroll){
				cast(spell_from_scroll);
				is_scroll = false;
			}else{
				battleMessage(action_message);
				if(manual_attack){
					manual_attack = false;
					monsterAttack();
				}
			}
		}else if(current_player->status == FROZEN || current_player->status == PARALYSED){
			switch(current_player->status){
				case FROZEN:
					battleMessage("You are frozen!");
					break;
				case PARALYSED:
					battleMessage("You are paralysed!");
					break;
			}
		}else{
		bch = getch();
		switch(bch){
			case 'd':
				battle_option = battle_option==3?0:battle_option+1;
				break;
			case 'a':
				battle_option = battle_option==0?3:battle_option-1;
				break;
			case '\n':
			case 'e':
				battleMenuSelect();
				break;
		}
		}
	}
}

void drawBattle(){
	//enemy info
	clear();
	switch(battle_option){
		case 0:
			mvaddch(43, 19, '>');
			break;
		case 1:
			mvaddch(43, 44, '>');
			break;
		case 2:
			mvaddch(43, 69, '>');
			break;
		case 3:
			mvaddch(43, 94, '>');
			break;
	}
	drawMonster();
	mvvline(40, 0, '#', 7);
	mvvline(40, 119,'#', 7);
	mvhline(40, 0, '#', 140);
	mvhline(46, 0, '#', 140);
	move(43, 20);
	printw("ATTACK");
	move(43, 45);
	printw("CAST");
	move(43, 70);
	printw("INVENTORY");
	move(43, 95);
	printw("FLEE");
}

void drawStats(){
	mvvline(40, 139,'#', 7);
	move(41, 121);
	if(player->status != NO_STATUS){
		switch(player->status){
			case POISONED:
				printw("[POISONED]");
		       		break;
			case SILENCED:
				printw("[SILENCED]");
		       		break;
			case PARALYSED:
				printw("[PARALYSED]");
		       		break;
			case BLEEDING:
				printw("[BLEEDING]");
				break;
			case BURNING:
				printw("[BURNING]");
				break;
			case FROZEN:
				printw("[FROZEN]");
				break;
		}
	}
	move(43, 121);
	printw("%s", player->name);
	move(44, 121);
	printw("HP: %d/%d", player->health < 0?0:player->health, player->max_health);
	move(45, 121);
	printw("MP: %d/%d", player->mana, player->max_mana);
}

void battleMenuSelect(){
	switch(battle_option){
		case 0:
			attack();
			break;
		case 1:
			displaySpells();
			break;
		case 2:
			drawInventory(true);
			break;
		case 3:
			flee();
			break;
	}
}

void attack(){
	int player_damage = 0;
	if((player->right_hand.weapon.name[0] == '\0' || player->right_hand.weapon.type == SHIELD) && (player->left_hand.weapon.name[0] == '\0' || player->left_hand.weapon.type == SHIELD)){
		player_damage = 1 + (player->stats.strength>0?roundf(player->stats.strength/2):0);
	}else{
		if(player->right_hand.weapon.type == BOW){
			if(player->arrows.name[0] == '\0'){
				battleMessage("You need arrows");
				return;
			}
			player_damage += getDamage(&player->right_hand.weapon, player);
			player_damage += getDamage(&player->arrows.weapon, player);
			player->arrows.weapon.count--;
			if(player->arrows.weapon.count == 0){
				battleMessage("That was your last arrow!");
				Item blank_item;
				blank_item.name[0] = '\0';
				blank_item.weapon.name[0] = '\0';
				blank_item.type = WEAPON;
				blank_item.weapon.count = 0;
				player->arrows = blank_item;
			}
		}else if(player->right_hand.weapon.type == THROWING_KNIVES){
			player_damage += getDamage(&player->right_hand.weapon, player);
			player->right_hand.weapon.count--;
			if(player->right_hand.weapon.count == 0){
				battleMessage("That was your last knife!");
				Item blank_item;
				blank_item.name[0] = '\0';
				blank_item.weapon.name[0] = '\0';
				blank_item.type = WEAPON;
				blank_item.weapon.count = 0;
				player->right_hand = blank_item;
			}
	}else{
		if(player->right_hand.weapon.type != SHIELD){
			player_damage += getDamage(&player->right_hand.weapon, player);
		}
		if(player->left_hand.weapon.type != SHIELD && player->left_hand.weapon.name[0] != '\0'){
			player_damage += getDamage(&player->left_hand.weapon, player);
		}
	}
	}
	timer_should_progress = true;
	monster->health -= player_damage;
	char attackMessage[100];
	if(monster->weakness == player->right_hand.weapon.element && player->right_hand.weapon.element != NONE){
		player_damage = roundf((float) player_damage * 1.5);
		sprintf(attackMessage, "%s is weak to %s!", monster->name, player->right_hand.weapon.name);
		battleMessage(attackMessage);
	}
	if(monster->resistance == player->right_hand.weapon.element && player->right_hand.weapon.element != NONE){
		player_damage = roundf((float) player_damage/2);
		sprintf(attackMessage, "%s is resistant to %s!", monster->name, player->right_hand.weapon.name);
		battleMessage(attackMessage);
	}
	sprintf(attackMessage, "Attacked for %d damage!", player_damage);
	battleMessage(attackMessage);
	if(player->right_hand.weapon.element == DRAIN){
		int health1 = player->health;
		player->health += getDamage(&player->right_hand.weapon, player);
		if(player->health > player->max_health){
			player->health = player->max_health;
		}
		int health2 = player->health;
		sprintf(attackMessage, "You restored %d health!", health2 - health1);
		battleMessage(attackMessage);
	}
	if(monster->health <= 0){
		sprintf(attackMessage, "You have killed %s!", monster->name);
		battleMessage(attackMessage);
		map[monster->pos.y][monster->pos.x].has_monster = false;
		map[monster->pos.y][monster->pos.x].ch = '.';
		player->status = NO_STATUS;
		monster->dead = true;
		int reward_exp = monster->exp;
		while(reward_exp > 0){
			if((player->exp + reward_exp) >= (player->lvl * 3)){
				reward_exp -= ((player->lvl * 3)-player->exp);
				player->exp = 0;
				player->skill_points++;
				player->lvl++;
				player->health = player->max_health;
				player->mana = player->max_mana;
				battleMessage("You have leveled up!");
			}else{
				player->exp += reward_exp;
				reward_exp = 0;
			}
		}
		clear();
		is_battling = false;
	}else{
			if((rand()%100) < 20){
				switch(player->right_hand.weapon.element){
					case FIRE:
						monster->status = BURNING;
						monster->status_timer = 3;
						sprintf(attackMessage, "%s has been burned!", monster->name);
						battleMessage(attackMessage);
						break;
					case SHOCK:
						monster->status = PARALYSED;
						monster->status_timer = (rand()%3) + 1;
						sprintf(attackMessage, "%s has been paralysed!", monster->name);
						battleMessage(attackMessage);
						break;
					case ICE:
						monster->status = FROZEN;
						monster->status_timer = 2;
						sprintf(attackMessage, "%s has been frozen!", monster->name);
						battleMessage(attackMessage);
						break;
				}
			}
			monsterAttack();
	}
}

void displaySpells(){
	int spell_index = 0;
	battle_state = CAST;
	while(battle_state == CAST){
		clear();
		drawMonster();
		mvvline(40, 0, '#', 7);
		mvvline(40, 119, '#', 7);
		mvhline(40, 0, '#', 140);
		mvhline(46, 0, '#', 140);
		drawStats();
		int pos = 0;
		int row = 41;
		int col = 5;
		for(int i = 0; i<(sizeof(player->spellbook)/sizeof(Spell)); i++){
			col = (5+((i%8)*12));
			if(i==8){
				row++;
			}else if(i==16){
				row++;
			}
			if(spell_selection == i){
				mvaddch(row, col-1, '>');
			}
			move(row, col);
			if(player->spellbook[i].name[0]!='\0'){
				if(spell_selection == i){
					mvaddch(row, col-1, '>');
					move(44, 5);
					if(player->spellbook[i].is_offensive && player->spellbook[i].element != NONE){
						printw("%s DMG: %d COST: %d -- E) Cast", player->spellbook[i].description, getSpellDamage(&player->spellbook[i], player), player->spellbook[i].mana_cost);
					}else if(player->spellbook[i].is_offensive){
						printw("%s COST: %d -- E) Cast", player->spellbook[i].description, player->spellbook[i].mana_cost);
					}else if(player->spellbook[i].type == HEALING){
						printw("%s HEALTH: %d COST: %d -- E) Cast", player->spellbook[i].description, getSpellDamage(&player->spellbook[i], player), player->spellbook[i].mana_cost);
					}else if(player->spellbook[i].type == PROTECT){
						printw("%s PROT: %d COST: %d -- E) Cast", player->spellbook[i].description, getSpellDamage(&player->spellbook[i], player), player->spellbook[i].mana_cost);
					}
				}
				move(row, col);
				printw("%s", player->spellbook[i].name);
			}
		}
		bch = getch();
		switch(bch){
			case 'q':
				battle_state = ATTACK;
				break;
			case 'w':
				if(spell_selection>7){
					spell_selection-=8;
				};
				break;
			case 's':
				if(spell_selection<16){
					spell_selection+=8;
				}
				break;
			case 'a':
				if(spell_selection > 0){
					spell_selection-=1;
				}
				break;
			case 'd':
				if(player->spellbook[spell_selection+1].name[0] != '\0'){
					spell_selection+=1;
				}
				break;
			case '\n':
			case 'e':
				cast(player->spellbook[spell_selection]);
		}
	}
}

void cast(Spell spell){
	if(spell.mana_cost <= player->mana){
		timer_should_progress = true;
	player->mana -= spell.mana_cost;
	char temp[100];
	sprintf(temp, "You cast %s...", spell.name);
	battleMessage(temp);
	if(spell.type == DAMAGE){
		int spell_damage = getSpellDamage(&spell, player);
		if(monster->weakness == spell.element && spell.element != NONE){
			spell_damage = roundf((float) spell_damage * 1.5);
			sprintf(temp, "%s is weak to %s!", monster->name, spell.name);
			battleMessage(temp);
		}
		if(monster->resistance == spell.element && spell.element != NONE){
			spell_damage = roundf((float) spell_damage/2);
			sprintf(temp, "%s is resistant to %s!", monster->name, spell.name);
			battleMessage(temp);
		}
		monster->health -= getSpellDamage(&spell, player);
		sprintf(temp, "%s was hurt for %d damage!", monster->name, getSpellDamage(&spell, player));
		battleMessage(temp);
		if(spell.element == DRAIN){
			int health1 = player->health;
			player->health += getSpellDamage(&spell, player);
			if(player->health > player->max_health){
				player->health = player->max_health;
			}
			int health2 = player->health;
			sprintf(temp, "You restored %d health!", health2 - health1);
			battleMessage(temp);
		}
	}else if(spell.type == INFLICT_STATUS){
		monster->status = spell.status;
		monster->status_timer = (rand()%(spell.tier * 3))+spell.tier;
		sprintf(temp, "%s was afflicted with Insanity", monster->name);
	}else if(spell.type == HEALING){
		int health1 = player->health;
		player->health += getSpellDamage(&spell, player);
		if(player->health > player->max_health){
			player->health = player->max_health;
		}
		int health2 = player->health;
		if(player->status == BLEEDING){
			player->status = NO_STATUS;
			sprintf(temp, "You restored %d health and are no longer bleeding!", health2 - health1);
		}else{
			sprintf(temp, "You restored %d health!", health2 - health1);
		}
		battleMessage(temp);
	}else if(spell.type == PROTECT){
		player->spell_protection = getSpellDamage(&spell, player);
		sprintf(temp, "You have created a magical barrier that will absorb %d damage!", player->spell_protection);
		battleMessage(temp);
	}
	char attackMessage[100];
	battle_state = ATTACK;
	if(monster->health > 0){
		if((rand()%100) < (spell.tier * 15)){
			switch(spell.element){
				case FIRE:
					monster->status = BURNING;
					monster->status_timer = 3;
					sprintf(attackMessage, "%s has been burned!", monster->name);
					battleMessage(attackMessage);
					break;
				case SHOCK:
					monster->status = PARALYSED;
					monster->status_timer = (rand()%3) + 1;
					sprintf(attackMessage, "%s has been paralysed!", monster->name);
					battleMessage(attackMessage);
					break;
				case ICE:
					monster->status = FROZEN;
					monster->status_timer = 2;
					sprintf(attackMessage, "%s has been frozen!", monster->name);
					battleMessage(attackMessage);
					break;
			}
		}
		monsterAttack();
	}else{
		sprintf(attackMessage, "You have killed %s!", monster->name);
		battleMessage(attackMessage);
		map[monster->pos.y][monster->pos.x].has_monster = false;
		map[monster->pos.y][monster->pos.x].ch = '.';
		player->status = NO_STATUS;
		monster->dead = true;
		int reward_exp = monster->exp;
		while(reward_exp > 0){
			if((player->exp + reward_exp) >= (player->lvl * 3)){
				reward_exp -= ((player->lvl * 3)-player->exp);
				player->exp = 0;
				player->skill_points++;
				player->lvl++;
				player->health = player->max_health;
				player->mana = player->max_mana;
				battleMessage("You have leveled up!");
			}else{
				player->exp += reward_exp;
				reward_exp = 0;
			}
		}
		clear();
		is_battling = false;
	}
	}else{
		battleMessage("You don't have enough mana!");
	}
}

void defend(){
	clear();
	is_defending = true;
	monsterAttack();
}

void flee(){
	timer_should_progress = true;
	clear();
	if((rand()%3)==0){
		battleMessage("You successfully escaped!");
		monster->wait_time = 3;
		clear();
		is_battling = false;
	}else{
		battleMessage("You failed to escape...");
		monsterAttack();
	}
}

void drawMonster(){
	mvvline(0, 0, '#', 7);
	mvvline(0, 139,'#', 7);
	mvhline(0, 0, '#', 140);
	mvhline(6, 0, '#', 140);
	move(2, 60);
	printw("%s", monster->name);
	move(4, 45);
	printw("HP: %d/%d", monster->health < 0?0:monster->health, monster->max_health);
	mvaddch(4, 59, '[');
	mvaddch(4, 80, ']' );
	for(int i = 0; i < 20; i++){
		if(monster->health >= roundf(((float)monster->max_health/20)*(i+1))){
			mvaddch(4, 59+i+1, '=');
		}
	}
	if(monster->status != NO_STATUS){
		move(3, 58);
		switch(monster->status){
			case POISONED:
				printw("[POISONED]");
		       		break;
			case SILENCED:
				printw("[SILENCED]");
		       		break;
			case PARALYSED:
				printw("[PARALYSED]");
		       		break;
			case BLEEDING:
				printw("[BLEEDING]");
				break;
			case BURNING:
				printw("[BURNING]");
				break;
			case FROZEN:
				printw("[FROZEN]");
				break;
			case INSANITY:
				printw("[INSANITY]");
				break;
		}
	}
	char monster_file[300];
	sprintf(monster_file, "src/monsters/%s.txt", monster->name);
	FILE *mptr = fopen(monster_file, "r");
	int fch;
	int col = 0;
	int row = 7;
	while((fch = fgetc(mptr))!=EOF){
		if(fch == '\n'){
			row++;
			col = 0;
			if(row > 39){
				break;
			}
		}else{
			mvaddch(row, col, fch);
			col++;
		}
	}
	fclose(mptr);
}

void battleMessage(char message[]){
	clear();
	drawMonster();
	drawStats();
	mvvline(40, 0, '#', 7);
	mvvline(40, 119,'#', 7);
	mvhline(40, 0, '#', 140);
	mvhline(46, 0, '#', 140);
	move(42, 2);
	printw("%s", message);
	getch();
}

void monsterAttack(){
	if(monster->status == PARALYSED){
		char temp[255];
		sprintf(temp, "%s is paralysed!", monster->name);
		battleMessage(temp);
	}else if(monster->status == FROZEN){
		char temp[255];
		sprintf(temp, "%s is frozen!", monster->name);
		battleMessage(temp);
	}else if(monster->status == INSANITY && (rand()%100 < 70)){
		int insanity_damage = (rand()%10)+1;
		monster->health -= insanity_damage;
		char insanity_message[100];
		sprintf(insanity_message, "%s hurt itself for %d damage!", monster->name, insanity_damage);
		battleMessage(insanity_message);
	}else{
	int attackChoice = rand()%monster->num_of_attacks;
	char monster_attack_message[255];
	if(monster->attacks[attackChoice].mana_cost > 0 && monster->status == SILENCED){
		sprintf(monster_attack_message, "%s tried to cast with %s, but failed", monster->name, monster->attacks[attackChoice].name);
	}else{
	if(monster->attacks[attackChoice].mana_cost > monster->mana){
		sprintf(monster_attack_message, "%s tried to cast with %s, but failed", monster->name, monster->attacks[attackChoice].name);
		battleMessage(monster_attack_message);
	}else{
		monster->mana -= monster->attacks[attackChoice].mana_cost;
		if((rand()%100) >= monster->attacks[attackChoice].accuracy){
			sprintf(monster_attack_message, "%s attacked with %s... but it missed!", monster->name, monster->attacks[attackChoice].name);
			battleMessage(monster_attack_message);
			return;
		}
		if((rand()%100) < 2+(player->stats.dexterity > 0?player->stats.dexterity/2:0)){
			sprintf(monster_attack_message, "%s attacked with %s... but you evaded it!", monster->name, monster->attacks[attackChoice].name);
			battleMessage(monster_attack_message);
			return;
		}
		if(player->spell_protection > 0){
			player->spell_protection -= monster->attacks[attackChoice].damage;
			sprintf(monster_attack_message, "%s attacked with %s but your protection spell absorbed the damage!", monster->name, monster->attacks[attackChoice].name);
			battleMessage(monster_attack_message);
			if(player->spell_protection <= 0){
				player->spell_protection = 0;
				battleMessage("Your protection spell has been broken!");
			}
		}else{
		int protectionFactor = 0;
		protectionFactor += player->head.armor.protection;
		protectionFactor += player->body.armor.protection;
		protectionFactor += player->hands.armor.protection;
		protectionFactor += player->legs.armor.protection;
		protectionFactor += player->feet.armor.protection;
		int prot_value;
		if(protectionFactor == 0){
			prot_value = 0;
		}else{
			prot_value = is_defending?protectionFactor:rand() % protectionFactor;
			if(player->left_hand.weapon.type == SHIELD){
				prot_value += player->left_hand.weapon.damage;
			}
		}
		prot_value += player->stats.endurence > 0?rand()%((int) (player->stats.endurence/2)):0;

		sprintf(monster_attack_message, "%s attacked with %s for %d damage!", monster->name, monster->attacks[attackChoice].name, monster->attacks[attackChoice].damage - prot_value > 0 ? monster->attacks[attackChoice].damage - prot_value : 0);
		player->health -= (monster->attacks[attackChoice].damage >= prot_value ? monster->attacks[attackChoice].damage - prot_value : 0);
		battleMessage(monster_attack_message);
		if(monster->attacks[attackChoice].status != NO_STATUS && (rand()%100 < monster->attacks[attackChoice].status_chance)){
			switch(monster->attacks[attackChoice].status){
				case POISONED:
					battleMessage("You have been poisoned!");
					player->status = POISONED;
			       		player->status_timer = (rand()%3)+4;
					break;
				case SILENCED:
					battleMessage("You have been silenced!");
					player->status = SILENCED;
					player->status_timer = (rand()%2)+2;
			       		break;
				case PARALYSED:
					battleMessage("You have been paralysed!");
					player->status = PARALYSED;
					player->status_timer = (rand()%2)+1;
			       		break;
				case BLEEDING:
					battleMessage("You are bleeding!");
					player->status = BLEEDING;
					player->status_timer = 3;
					printw("[BLEEDING]");
					break;
				case BURNING:
					battleMessage("You are burning!");
					player->status = BURNING;
					player->status_timer = (rand()%4)+1;
					break;
				case FROZEN:
					battleMessage("You have been frozen!");
					player->status = FROZEN;
					player->status_timer = (rand()%2)+2;
					break;
				case INSANITY:
					battleMessage("You have been driven insane!");
					player->status = INSANITY;
					player->status_timer = (rand()%5)+1;
					break;
			}
		}
		if(player->health <= 0){
			battleMessage("You died...");
			is_battling = false;
			death();
		}
		}
		//add status infliction here
	}
	}
	}
}
