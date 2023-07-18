#include <stdio.h>
#include <ncurses.h>
#include "player.h"
#include <stdlib.h>
#include "ui.h"
#include <math.h>

int skill_points = 3;
Stats *stats;
char player_name[255];
int name_index = 0;
Player *current_player;

bool in_inventory = false;

bool action_taken = false;
char action_message[255];
Spell spell_from_scroll;
bool is_scroll = false;
bool is_battling = false;
bool manual_attack = false;
bool focused = false;

int num_of_items = 0;
int num_of_spells = 0;

void drawMenu(int selected){
	clear();
	FILE *fptr = fopen("./src/title.txt", "r");
	if(fptr == NULL){
		return;
	}
	int col = 3;
	int row = 2;
	int fileCh;
	mvvline(0, 0, '#', 12);
	mvvline(0, 57, '#', 12);
	mvhline(0, 0, '#', 57);
	mvhline(11, 0, '#', 57);
	while((fileCh=fgetc(fptr))!=EOF){
		if(fileCh=='\n'){
			row++;
			col = 3;
		}else{
			mvaddch(row, col, fileCh);
			col++;
		}
	}
	if(selected==0){
		mvaddch(15,2,'>');
	}
	mvaddstr(15,3,"New Game");
	if(selected==1){
		mvaddch(17,2,'>');
	}
	mvaddstr(17,3,"Load Game");
	if(selected==2){
		mvaddch(19,2,'>');
	}
	mvaddstr(19,3,"Exit");
}

void initializeCharacterMenu(){
	for(int i = 0; i <= name_index; i++){
		player_name[i] = '\0';
	}
	name_index = 0;
	skill_points = 3;
	stats = calloc(1, sizeof(Stats));
	stats->constitution = 0;
	stats->strength = 0;
	stats->dexterity = 0;
	stats->intelligence = 0;
	stats->endurence = 0;
	stats->wisdom = 0;
}

void drawCharacterMenu(int selected){
	clear();
	move(3, 3);
	printw("Name: %s", player_name);
	switch(selected){
		case 0:
		mvaddch(3, 2, '>');
		break;
		case 1:
		mvaddch(12, 22, '<');
		mvaddch(12, 24, '>');
		break;
		case 2:
		mvaddch(14, 22, '<');
		mvaddch(14, 24, '>');
		break;
		case 3:
		mvaddch(16, 22, '<');
		mvaddch(16, 24, '>');
		break;
		case 4:
		mvaddch(18, 22, '<');
		mvaddch(18, 24, '>');
		break;
		case 5:
		mvaddch(20, 22, '<');
		mvaddch(20, 24, '>');
		break;
		case 6:
		mvaddch(22, 22, '<');
		mvaddch(22, 24, '>');
		break;
		case 7:
		mvaddch(26, 9, '>');
		break;
		case 8:
		mvaddch(28, 9, '>');
		break;
	}
	move(10, 3);
       	printw("Stats           Skill Points: %d",skill_points );
	mvaddstr(12, 3, "Constitution");
	move(12, 23);
	printw("%d", stats->constitution);
	mvaddstr(14, 3, "Strength");
	move(14, 23);
	printw("%d", stats->strength);
	mvaddstr(16, 3, "Dexterity");
	move(16, 23);
	printw("%d", stats->dexterity);
	mvaddstr(18, 3, "Intelligence");
	move(18, 23);
	printw("%d", stats->intelligence);
	mvaddstr(20, 3, "Endurance");
	move(20, 23);
	printw("%d", stats->endurence);
	mvaddstr(22, 3, "Attunement");
	move(22, 23);
	printw("%d", stats->wisdom);
	mvaddstr(26, 10, "Confirm");
	mvaddstr(28, 10, "Cancel");
	char tooltip[255];
	bool print_tooltip = true;
	switch(selected){
		case 0:
			sprintf(tooltip, "%s", focused?"Type to enter a name, press TAB or ENTER to confirm name":"Press E or ENTER to enter name");
			break;
		case 1:
			sprintf(tooltip, "Increases health, press A to remove a skill point and D to assign a skill point");
			break;
		case 2:
			sprintf(tooltip, "Increases damage with strength weapons, press A to remove a skill point and D to assign a skill point");
			break;
		case 3:
			sprintf(tooltip, "Increases damage with dexterity weapons, press A to remove a skill point and D to assign a skill point");
		       	break;
		case 4:
			sprintf(tooltip, "Increases spell damage, press A to remove a skill point and D to assign a skill point");
			break;
		case 5:
			sprintf(tooltip, "Increases defence, press A to remove a skill point and D to assign a skill point");
			break;
		case 6:
			sprintf(tooltip, "Increases mana, press A to remove a skill point and D to assign a skill point");
			break;
		default:
			print_tooltip = false;
			break;
		}
	if(print_tooltip){
		move(32, 5);
		printw("%s", tooltip);
	}
}


void addSkillPoint(int skill){
	if(skill_points>0){
		switch(skill){
			case 1:
				stats->constitution++;
				break;
			case 2:
				stats->strength++;
				break;
			case 3:
				stats->dexterity++;
				break;
			case 4:
				stats->intelligence++;
				break;
			case 5:
				stats->endurence++;
				break;
			case 6:
				stats->wisdom++;
				break;
		}
		skill_points--;
	}

}


void removeSkillPoint(int skill){
		switch(skill){
			case 1:
				if(stats->constitution>0){
					stats->constitution--;
					skill_points++;
				}
				break;
			case 2:
				if(stats->strength>0){
					stats->strength--;
					skill_points++;
				}
				break;
			case 3:
				if(stats->dexterity>0){
					stats->dexterity--;
					skill_points++;
				}
				break;
			case 4:
				if(stats->intelligence>0){
					stats->intelligence--;
					skill_points++;
				}
				break;
			case 5:
				if(stats->endurence>0){
					stats->endurence--;
					skill_points++;
				};
				break;
			case 6:
				if(stats->wisdom>0){
					stats->wisdom--;
					skill_points++;
				};
				break;
		}

}

void nameFocus(){
	int namech;
	focused = true;
	while(focused){
		drawCharacterMenu(0);
		mvaddch(3, 69, '<');
		namech = getch();
		if(namech == 127 || namech == 8){
			if(name_index > 0){
				player_name[name_index-1]='\0';
				name_index--;
			}else{
				focused = false;
			}
		}else if(namech == '\n' || namech == '\t'){
			focused = false;
		}else{
			if(name_index<60){
				player_name[name_index]=namech;
				name_index++;
			}
		}
	}
}

void characterConfirmation(){
	current_player = createPlayer(player_name, stats);
	current_player->skill_points = skill_points;
	free(stats);
}

void inventorySort(){
	for(int i = 0; i < num_of_items; i++){
		for(int j = 0; j < num_of_items; j++){
			if(j < num_of_items - 1){
				if(current_player->inventory[j].name[0] > current_player->inventory[j+1].name[0]){
					Item temp_item = current_player->inventory[j+1];
					current_player->inventory[j+1] = current_player->inventory[j];
					current_player->inventory[j] = temp_item;
				}
			}
		}
	}
	for(int i = 0; i < num_of_spells; i++){
		for(int j = 0; j < num_of_spells; j++){
			if(j < num_of_spells - 1){
				if(current_player->spellbook[j].name[0] > current_player->spellbook[j+1].name[0]){
					Spell temp_spell = current_player->spellbook[j+1];
					current_player->spellbook[j+1] = current_player->spellbook[j];
					current_player->spellbook[j] = temp_spell;
				}
			}
		}
	}
}

void drawInventory(){
	in_inventory = true;
	int ich;
	int sub_section = 2;
	int index = 0;
	int protectionFactor = 0;
	while(in_inventory){
		inventorySort();
		protectionFactor = 0;
		protectionFactor += current_player->head.name[0] != '\0'?current_player->head.armor.protection:0;
		protectionFactor += current_player->body.name[0] != '\0'?current_player->body.armor.protection:0;
		protectionFactor += current_player->hands.name[0] != '\0'?current_player->hands.armor.protection:0;
		protectionFactor += current_player->legs.name[0] != '\0'?current_player->legs.armor.protection:0;
		protectionFactor += current_player->feet.name[0] != '\0'?current_player->feet.armor.protection:0;
		protectionFactor += current_player->off_hand.weapon.type == SHIELD?current_player->off_hand.weapon.damage:0;
		protectionFactor += (int) (current_player->stats.endurence/2);
		num_of_spells = 0;
		num_of_items = 0;
		clear();
		mvvline(0, 0, '#', 47);
		mvvline(0, 119, '#', 47);
		mvhline(0, 0, '#', 120);
		mvhline(40, 0, '#', 120);
		mvhline(46, 0, '#', 120);
		mvvline(0, 39, '#', 40);
		mvvline(0, 79, '#', 40);
		mvhline(19, 0, '#', 40);
		move(2, 17);
		printw("STATS");
		move(4, 2);
		printw("%s", current_player->name);
		move(5, 2);
		printw("LVL: %d", current_player->lvl);
		move(6, 2);
		printw("XP: %d/%d", current_player->exp, current_player->lvl*3);
		move(5, 15);
		printw("Skill Points: %d", current_player->skill_points);
		mvaddch(6,15,'[');
		mvaddch(6,36, ']');
		float max_xp = current_player->lvl*3;
		for(int i = 0; i < 20; i++){
			if(current_player->exp >= ((max_xp/20)*(i+1))){
				mvaddch(6,15+i+1, '=');
			}
		}
		move(7, 2);
		printw("HP: %d/%d", current_player->health, current_player->max_health);
		move(8, 2);
		printw("MP: %d/%d", current_player->mana, current_player->max_mana);
		move(10, 2);
		printw("Constitution: ");
		move(10, 25);
		printw("%d", current_player->stats.constitution);
		move(11, 2);
		printw("Strength: ");
		move(11, 25);
		printw("%d", current_player->stats.strength);
		move(12, 2);
		printw("Dexterity: ");
		move(12, 25);
		printw("%d", current_player->stats.dexterity);
		move(13, 2);
		printw("Intelligence: ");
		move(13, 25);
		printw("%d", current_player->stats.intelligence);
		move(14, 2);
		printw("Endurance: ");
		move(14, 25);
		printw("%d", current_player->stats.endurence);
		move(15, 2);
		printw("Attunement: ");
		move(15, 25);
		printw("%d", current_player->stats.wisdom);

		move(21, 15);
		printw("EQUIPMENT");
		move(24, 2);


		int player_damage = 0;
		if((current_player->main_hand.weapon.name[0] == '\0' || current_player->main_hand.weapon.type == SHIELD) && (current_player->off_hand.weapon.name[0] == '\0' || current_player->off_hand.weapon.type == SHIELD)){
			player_damage = 1 + (current_player->stats.strength>0?roundf(current_player->stats.strength/2):0);
		}else{
			if(current_player->main_hand.weapon.type != SHIELD && current_player->main_hand.weapon.name[0] != '\0'){
				player_damage += getDamage(&current_player->main_hand.weapon, current_player);
			}
			if(current_player->off_hand.weapon.type != SHIELD && current_player->off_hand.weapon.name[0] != '\0'){
				player_damage += getDamage(&current_player->off_hand.weapon, current_player);
			}
			if(current_player->arrows.weapon.name[0] != '\0' && current_player->main_hand.type == BOW){
				player_damage += getDamage(&current_player->arrows.weapon, current_player);
			}
		}

		printw("ATTACK: %d", player_damage);
		move(25, 2);
		printw("DEFENCE: %d", protectionFactor);
		move(26, 2);
		char arrowMessage[100];
		sprintf(arrowMessage, " (%d)", current_player->arrows.weapon.count);
		printw("Arrows: %s%s", current_player->arrows.name, current_player->arrows.weapon.count > 0?arrowMessage:"");
		move(28, 2);
		char weaponRightCount[100];
		sprintf(weaponRightCount, " (%d)", current_player->main_hand.weapon.count);
		printw("Main Hand: %s%s", current_player->main_hand.name, current_player->main_hand.weapon.count > 0?weaponRightCount:"");
		move(29, 2);
		char weaponLeftCount[100];
		sprintf(weaponLeftCount, " (%d)", current_player->off_hand.weapon.count);
		printw("Off Hand: %s%s", current_player->off_hand.name, current_player->off_hand.weapon.count > 0?weaponLeftCount:"");
		move(32, 2);
		printw("Head: %s", current_player->head.name);
		move(33, 2);
		printw("Body: %s", current_player->body.name);
		move(34, 2);
		printw("Hands: %s", current_player->hands.name);
		move(35, 2);
		printw("Legs: %s", current_player->legs.name);
		move(36, 2);
		printw("Feet: %s", current_player->feet.name);
		move(2, 54);
		printw("INVENTORY");
		for(int i = 0; i < (sizeof(current_player->inventory)/sizeof(Item)); i++){
			if(current_player->inventory[i].name[0] != '\0'){
				move(5+(i*2), 43);
				if(current_player->inventory[i].type == WEAPON){
					if(current_player->inventory[i].weapon.type == THROWING_KNIVES || current_player->inventory[i].weapon.type == ARROWS){
						printw("%s (%d)", current_player->inventory[i].name, current_player->inventory[i].weapon.count);
					}else{
						printw("%s", current_player->inventory[i].name);
					}
				}else{
					printw("%s", current_player->inventory[i].name);
				}
				num_of_items++;
			}else{
				break;
			}
		}
		move(2, 95);
		printw("SPELLBOOK");
		for(int i = 0; i < (sizeof(current_player->spellbook)/sizeof(Spell)); i++){
			if(current_player->spellbook[i].name[0] != '\0'){
				move(5+(i*2), 83);
				printw("%s", current_player->spellbook[i].name);
				num_of_spells++;
			}
		}
		if(action_taken){
			move(43, 2);
			printw("%s", action_message);
			action_taken = false;
			getch();
		}

		switch(sub_section){
			case 0:
				mvaddch(10+index, 27, '<');
				mvaddch(10+index, 23, '>');
				move(43, 2);
				char skillDesc[6][100] = {"Increases health", "Increases damage with strength weapons", "Increases damage with dexterity weapons", "Increases spell damage", "Increases defence", "Increases mana"};
				printw("%s -- E) Assign Skill Point", skillDesc[index]);
				break;
			case 1:
				if(index == 0){
					mvaddch(26, 1, '>');
				}else if(index == 1){
					mvaddch(28, 1, '>');
				}else if(index == 2){
					mvaddch(29, 1, '>');
				}else if(index > 2){
					mvaddch(32 + (index-3) , 1, '>');
				}
				Item * current_item = calloc(1, sizeof(Item));
				current_item->name[0] = '\0';
				switch(index){
					case 0:
						if(current_player->arrows.name[0] != '\0'){
							current_item = &current_player->arrows;
						}
						break;
					case 1:
						if(current_player->main_hand.name[0] != '\0'){
							current_item = &current_player->main_hand;
						}
						break;
					case 2:
						if(current_player->off_hand.name[0] != '\0'){
							current_item = &current_player->off_hand;
						}
						break;
					case 3:
						if(current_player->head.name[0] != '\0'){
							current_item = &current_player->head;
						}
						break;
					case 4:
						if(current_player->body.name[0] != '\0'){
							current_item = &current_player->body;
						}
						break;
					case 5:
						if(current_player->hands.name[0] != '\0'){
							current_item = &current_player->hands;
						}
						break;
					case 6:
						if(current_player->legs.name[0] != '\0'){
							current_item = &current_player->legs;
						}
						break;
					case 7:
						if(current_player->feet.name[0] != '\0'){
							current_item = &current_player->feet;
						}
						break;
				}
				move(43, 2);
				if(current_item->name[0] != '\0'){
					if(current_item->type == WEAPON && current_item->weapon.type != SHIELD){
						printw("%s DMG: %d -- E) Unequip", current_item->description, getDamage(&current_item->weapon, current_player));
					}else{
						printw("%s DEF: %d -- E) Unequip", current_item->description, current_item->type == ARMOR?current_item->armor.protection:getDamage(&current_item->weapon, current_player));
					}
				}
				break;
			case 2:
				mvaddch(5+(index*2), 42, '>');
				move(43, 2);
				if(current_player->inventory[index].name[0] != '\0'){
					if(current_player->inventory[index].type == WEAPON && current_player->inventory[index].weapon.type != SHIELD){
						printw("%s DMG: %d -- E) Equip Main Hand %sX) Drop", current_player->inventory[index].description, getDamage(&current_player->inventory[index].weapon, current_player), current_player->inventory[index].weapon.is_two_handed?"":current_player->main_hand.name[0]!='\0'&& !(current_player->main_hand.weapon.is_two_handed)?"R) Equip Off Hand ":"");
					}else if(current_player->inventory[index].type == CONSUMABLE){
						if(current_player->inventory[index].consumable.type == SCROLL){
							if(current_player->inventory[index].consumable.scroll_spell.type == HEALING){
								printw("%s HEALTH: %d -- E) USE X) Drop", current_player->inventory[index].description, getSpellDamage(&current_player->inventory[index].consumable.scroll_spell, current_player));
							}else if(current_player->inventory[index].consumable.scroll_spell.type == DAMAGE){
								printw("%s DMG: %d -- X) Drop", current_player->inventory[index].consumable.scroll_spell.description, getSpellDamage(&current_player->inventory[index].consumable.scroll_spell, current_player));
							}else if(current_player->inventory[index].consumable.scroll_spell.type == PROTECT){
								printw("%s PROT: %d -- X) Drop", current_player->inventory[index].consumable.scroll_spell.description, getSpellDamage(&current_player->inventory[index].consumable.scroll_spell, current_player));
							}else{
								printw("%s -- X) Drop", current_player->inventory[index].consumable.scroll_spell.description);
							}
						}else if(current_player->inventory[index].consumable.type == BOOK_OF_KNOWLEDGE){
							printw("%s -- %s", current_player->inventory[index].description, is_battling?"X) Drop":"E) Use X) Drop");
						}else{
							printw("%s -- E) USE X) Drop", current_player->inventory[index].description);
						}
					}else{
						printw("%s DEF: %d -- E) Equip X) Drop", current_player->inventory[index].description, current_player->inventory[index].type == ARMOR?current_player->inventory[index].armor.protection:getDamage(&current_player->inventory[index].weapon, current_player));
					}
				}
				break;
			case 3:
				mvaddch(5+(index*2), 82, '>');
				move(43, 2);
				if(current_player->spellbook[index].name[0] != '\0'){
					if(current_player->spellbook[index].is_offensive && current_player->spellbook[index].element != NONE){
						printw("%s DMG: %d COST: %d -- X) Forget Spell", current_player->spellbook[index].description, getSpellDamage(&current_player->spellbook[index], current_player), current_player->spellbook[index].mana_cost);
					}else if(current_player->spellbook[index].is_offensive){
						printw("%s COST: %d -- X) Forget Spell", current_player->spellbook[index].description, current_player->spellbook[index].mana_cost);
					}else if(current_player->spellbook[index].type == HEALING){
						printw("%s HEALTH: %d COST: %d -- %s", current_player->spellbook[index].description, getSpellDamage(&current_player->spellbook[index], current_player), current_player->spellbook[index].mana_cost, is_battling?"X) Forget Spell":"E) Cast X) Forget Spell");
					}else if(current_player->spellbook[index].type == PROTECT){
						printw("%s PROT: %d COST: %d -- X) Forget Spell", current_player->spellbook[index].description, getSpellDamage(&current_player->spellbook[index], current_player), current_player->spellbook[index].mana_cost);
					}
				}
				break;
		}
		ich = getch();
		switch(ich){
			case 'q':
				in_inventory = false;
				break;
			case 'w':
				if(sub_section == 0){
					if(index > 0){
						index--;
					}
				}else if(sub_section == 1){
					if(index == 0 && current_player->skill_points > 0){
						index = 5;
						sub_section--;
					}else if(index > 0){
						index--;
					}
				}else if(sub_section == 2 || sub_section == 3){
					if(index > 0){
						index--;
					}
				}
				break;
			case 's':
				if(sub_section == 0){
					if(index < 5){
						index++;
					}else{
						index = 0;
						sub_section++;
					}
				}else if(sub_section == 1){
					if(index < 7){
						index++;
					}
				}else if(sub_section == 2){
					if(index < num_of_items - 1){
						index++;
					}
				}else if(sub_section == 3){
					if(index < num_of_spells - 1){
						index++;
					}
				}
				break;
			case '\n':
			case 'e':
				inventorySelect(sub_section, index);
				break;
			case 'r':
				if(sub_section == 2 && current_player->inventory[index].type == WEAPON && current_player->main_hand.name[0]!='\0' && !(current_player->main_hand.weapon.is_two_handed)){
					if(current_player->inventory[index].weapon.type != SHIELD && !(current_player->inventory[index].weapon.is_two_handed)){
						equipOffHand(index);
					}
				}
				break;
			case 'a':
				if(sub_section == 2){
					index = 0;
					if(current_player->skill_points > 0){
						sub_section = 0;
					}else{
						sub_section = 1;
					}
				}else if(sub_section == 3){
					index = 0;
					sub_section = 2;
				}
				break;
			case 'd':
				if(sub_section < 3){
					sub_section = sub_section == 0 || sub_section == 1?2:3;
					index = 0;
				}
				break;
			case 'x':
				if(sub_section == 2){
					for(int i = index; i < num_of_items; i++){
						if(i == 17){
							current_player->inventory[i].name[0] = '\0';
						}else{
							current_player->inventory[i] = current_player->inventory[i+1];
						}
					}
					num_of_items--;
				}else if(sub_section == 3){
					for(int i = index; i < num_of_spells; i++){
						if(i == 17){
							current_player->spellbook[i].name[0] = '\0';
						}else{
							current_player->spellbook[i] = current_player->spellbook[i+1];
						}
					}
					num_of_spells--;
				}
				break;
		}
	}
}

void equipOffHand(int index){
		bool item_swapped = false;
		bool item_2_swapped = false;
		Item temp_item;
		Item temp_item_2;
		Item blank_item;
		strcpy(blank_item.name, "\0");
		strcpy(blank_item.armor.name, "\0");
		strcpy(blank_item.weapon.name, "\0");
		blank_item.weapon.damage = 0;
		blank_item.armor.protection = 0;
		blank_item.weapon.count = 0;
		blank_item.weapon.is_two_handed = false;
		if(current_player->off_hand.name[0] != '\0'){
			temp_item = current_player->off_hand;
			item_swapped = true;
		}
		current_player->off_hand = current_player->inventory[index];
		current_player->inventory[index] = blank_item;
		num_of_items--;
		for(int i = index; i < num_of_items; i++){
			if(i == 17){
				current_player->inventory[i].name[0] = '\0';
			}else{
				current_player->inventory[i] = current_player->inventory[i+1];
				current_player->inventory[i+1] = blank_item;
			}
		}
		if(item_swapped){
			num_of_items++;
			current_player->inventory[num_of_items-1] = temp_item;
		}
}

void inventorySelect(int sub_section, int index){
	Item blank_item;
	strcpy(blank_item.name, "\0");
	strcpy(blank_item.armor.name, "\0");
	strcpy(blank_item.weapon.name, "\0");
	blank_item.weapon.damage = 0;
	blank_item.armor.protection = 0;
	blank_item.weapon.count = 0;
	blank_item.weapon.is_two_handed = false;
	switch(sub_section){
		case 0:
			if(current_player->skill_points > 0){
				switch(index){
					case 0:
						current_player->stats.constitution++;
						current_player->max_health = 10+(2*current_player->stats.constitution);
						current_player->health += 2;
						break;
					case 1:
						current_player->stats.strength++;
						break;
					case 2:
						current_player->stats.dexterity++;
						break;
					case 3:
						current_player->stats.intelligence++;
						break;
					case 4:
						current_player->stats.endurence++;
						break;
					case 5:
						current_player->stats.wisdom++;
						current_player->max_mana = 2*current_player->stats.wisdom;
						current_player->mana += 2;
						break;
				}
				current_player->skill_points--;
				if(current_player->skill_points == 0){
					sub_section = 2;
					index = 0;
				}
			}
			break;
		case 1:
			switch(index){
				case 0:
					if(current_player->arrows.name[0] != '\0'){
						num_of_items++;
						current_player->inventory[num_of_items-1] = current_player->arrows;
						current_player->arrows = blank_item;
					}
					break;
				case 1:
					if(current_player->main_hand.name[0] != '\0'){
						num_of_items++;
						current_player->inventory[num_of_items-1] = current_player->main_hand;
						current_player->main_hand = blank_item;
					}
					break;
				case 2:
					if(current_player->off_hand.name[0] != '\0'){
						num_of_items++;
						current_player->inventory[num_of_items-1] = current_player->off_hand;
						current_player->off_hand = blank_item;
					}
					break;
				case 3:
					if(current_player->head.name[0] != '\0'){
						num_of_items++;
						current_player->inventory[num_of_items-1] = current_player->head;
						current_player->head = blank_item;
					}
					break;
				case 4:
					if(current_player->body.name[0] != '\0'){
						num_of_items++;
						current_player->inventory[num_of_items-1] = current_player->body;
						current_player->body = blank_item;
					}
					break;
				case 5:
					if(current_player->hands.name[0] != '\0'){
						num_of_items++;
						current_player->inventory[num_of_items-1] = current_player->hands;
						current_player->hands = blank_item;
					}
					break;
				case 6:
					if(current_player->legs.name[0] != '\0'){
						num_of_items++;
						current_player->inventory[num_of_items-1] = current_player->legs;
						current_player->legs = blank_item;
					}
					break;
				case 7:
					if(current_player->feet.name[0] != '\0'){
						num_of_items++;
						current_player->inventory[num_of_items-1] = current_player->feet;
						current_player->feet = blank_item;
					}
					break;
			}
			break;
		case 2:
			bool item_swapped = false;
			bool item_2_swapped = false;
			Item temp_item;
			Item temp_item_2;
			if(current_player->inventory[index].type == WEAPON){
				if(current_player->inventory[index].weapon.type == ARROWS){
					if(current_player->arrows.name[0] != '\0'){
						temp_item = current_player->arrows;
						item_swapped = true;
					}
					current_player->arrows.weapon.is_equipped = false;
					current_player->arrows = current_player->inventory[index];
				}else if(current_player->inventory[index].weapon.type == SHIELD){
					equipOffHand(index);
					return;
				}else{
					if(current_player->inventory[index].weapon.is_two_handed){
						if(num_of_items > 15 && current_player->off_hand.name[0]!='\0'){
							move(43, 2);
							printw("There is not enough room in your inventory!");
						}else{
							if(current_player->main_hand.name[0] != '\0'){
								temp_item = current_player->main_hand;
								item_swapped = true;
							}
							current_player->main_hand = current_player->inventory[index];
							current_player->inventory[index] = blank_item;
							num_of_items--;
							if(current_player->off_hand.name[0] != '\0'){
								temp_item_2 = current_player->off_hand;
								item_2_swapped = true;
								current_player->off_hand = blank_item;
							}
						}
					}else{
						if(current_player->main_hand.name[0] != '\0'){
							temp_item = current_player->main_hand;
							item_swapped = true;
						}
						current_player->main_hand = current_player->inventory[index];
						current_player->inventory[index] = blank_item;
						num_of_items--;
					}
					for(int i = index; i < num_of_items; i++){
						if(i == 17){
							current_player->inventory[i].name[0] = '\0';
						}else{
							current_player->inventory[i] = current_player->inventory[i+1];
							current_player->inventory[i+1] = blank_item;
						}
					}
					if(item_swapped){
						num_of_items++;
						current_player->inventory[num_of_items-1] = temp_item;
					}
					if(item_2_swapped){
						num_of_items++;
						current_player->inventory[num_of_items-1] = temp_item_2;
					}
				}
			}else if(current_player->inventory[index].type == ARMOR){
				switch(current_player->inventory[index].armor.type){
					case HEAD:
						if(current_player->head.name[0] != '\0'){
							temp_item = current_player->head;
							item_swapped = true;
						}
						current_player->head.armor.is_equipped = false;
						current_player->head = current_player->inventory[index];
						break;
					case BODY:
						if(current_player->body.name[0] != '\0'){
							temp_item = current_player->body;
							item_swapped = true;
						}
						current_player->body.armor.is_equipped = false;
						current_player->body = current_player->inventory[index];
						//current_player->body.armor.is_equipped = true;
						break;
					case HANDS:
						if(current_player->hands.name[0] != '\0'){
							temp_item = current_player->hands;
							item_swapped = true;
						}
						current_player->hands.armor.is_equipped = false;
						current_player->hands = current_player->inventory[index];
						//current_player->hands.armor.is_equipped = true;
						break;
					case LEGS:
						if(current_player->legs.name[0] != '\0'){
							temp_item = current_player->legs;
							item_swapped = true;
						}
						current_player->legs.armor.is_equipped = false;
						current_player->legs = current_player->inventory[index];
						//current_player->legs.armor.is_equipped = true;
						break;
					case FEET:
						if(current_player->feet.name[0] != '\0'){
							temp_item = current_player->feet;
							item_swapped = true;
						}
						current_player->feet.armor.is_equipped = false;
						current_player->feet = current_player->inventory[index];
						//current_player->feet.armor.is_equipped = true;
						break;
				}
				current_player->inventory[index] = blank_item;
				num_of_items--;
				for(int i = index; i < num_of_items; i++){
					if(i == 17){
						current_player->inventory[i].name[0] = '\0';
					}else{
						current_player->inventory[i] = current_player->inventory[i+1];
						current_player->inventory[i+1] = blank_item;
					}
				}
				if(item_swapped){
					num_of_items++;
					current_player->inventory[num_of_items-1] = temp_item;
				}
			}else if(current_player->inventory[index].type == CONSUMABLE){
				if(is_battling){
					switch(current_player->inventory[index].consumable.type){
						case HEALTH_POTION:
							action_taken = true;
							manual_attack = true;
							int health1 = current_player->health;
							current_player->health += current_player->inventory[index].consumable.tier*10;
							if(current_player->health > current_player->max_health){
								current_player->health = current_player->max_health;
							}
							int health2 = current_player->health;
							sprintf(action_message, "%d health restore", health2 - health1);
							in_inventory = false;
							break;
						case MANA_POTION:
							action_taken = true;
							manual_attack = true;
							int mana1 = current_player->mana;
							current_player->mana += current_player->inventory[index].consumable.tier*10;
							if(current_player->mana > current_player->max_mana){
								current_player->mana = current_player->max_mana;
							}
							int mana2 = current_player->mana;
							sprintf(action_message, "%d mana restored!", mana2 - mana1);
							in_inventory = false;
							break;
						case SCROLL:
							action_taken = true;
							manual_attack = false;
							is_scroll = true;
							spell_from_scroll = current_player->inventory[index].consumable.scroll_spell;
							in_inventory = false;
							break;
					}
				}else{
					switch(current_player->inventory[index].consumable.type){
						case HEALTH_POTION:
							int health1 = current_player->health;
							current_player->health += current_player->inventory[index].consumable.tier*10;
							if(current_player->health > current_player->max_health){
								current_player->health = current_player->max_health;
							}
							int health2 = current_player->health;
							sprintf(action_message, "%d health restored!", health2 - health1);
							action_taken = true;
							break;
						case MANA_POTION:
							int mana1 = current_player->mana;
							current_player->mana += current_player->inventory[index].consumable.tier*10;
							if(current_player->mana > current_player->max_mana){
								current_player->mana = current_player->max_mana;
							}
							int mana2 = current_player->mana;
							sprintf(action_message, "%d mana restored!", mana2 - mana1);
							action_taken = true;
							break;
						case SCROLL:
							if(current_player->inventory[index].consumable.scroll_spell.type == HEALING){
							int health1 = current_player->health;
							current_player->health += getSpellDamage(&current_player->inventory[index].consumable.scroll_spell, current_player);
							if(current_player->health > current_player->max_health){
								current_player->health = current_player->max_health;
							}
							int health2 = current_player->health;
							sprintf(action_message, "%d health restored!", health2 - health1);
							action_taken = true;
							}
							break;
						case BOOK_OF_KNOWLEDGE:
							current_player->skill_points++;
							action_taken = true;
							break;
						default:
							break;
					}
				}
				if(action_taken){
					for(int i = index; i < num_of_items; i++){
						if(i == 17){
							current_player->inventory[i].name[0] = '\0';
						}else{
							current_player->inventory[i] = current_player->inventory[i+1];
						}
					}
					num_of_items--;
					if(index > 0){
						index--;
					}
				}
			}
			break;
		case 3:
			if(!is_battling){
				if(current_player->spellbook[index].type == HEALING){
				move(50, 0);
					if(current_player->spellbook[index].mana_cost <= current_player->mana){
						int health1 = current_player->health;
						current_player->health += getSpellDamage(&current_player->spellbook[index], current_player);
						if(current_player->health > current_player->max_health){
							current_player->health = current_player->max_health;
						}
						int health2 = current_player->health;
						sprintf(action_message, "%d health restored!", health2 - health1);
						current_player->mana -= current_player->spellbook[index].mana_cost;
					}else{
						sprintf(action_message, "You don't have enough mana!");
					}
					action_taken = true;
				}
			}
			break;
	}
}
