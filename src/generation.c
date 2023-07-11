#include <time.h>
#include <stdlib.h>
#include "generation.h"
#include <ncurses.h>
#include <string.h>
#include <math.h>
#include "battle.h"
#include "ui.h"

Tile**map;
Monster monsters[100];
int monster_index = 0;
//Monster monsters[30];
//int monster_index = 0;
//int num_of_monsters = 0;
int doorway = 1;


Room createRoom(int y, int x, int height, int width){
	Room newRoom;
	newRoom.pos.y = y;
	newRoom.pos.x = x;
	newRoom.height = height;
	newRoom.width = width;
	newRoom.center.y = y+(int)(height/2);
	newRoom.center.x = x+(int)(width/2);
	return newRoom;
}

void addRoom(Room room){
	int items = (rand() % 2);
	int enemies = (rand() % 3);
	for(int y = room.pos.y; y < room.pos.y + room.height; y++){
		for(int x = room.pos.x; x < room.pos.x + room.width; x++){
			if((rand () % 100) == 0 && items > 0){
				items --;
				map[y][x].ch = 'i';
				map[y][x].has_item = true;
				map[y][x].has_monster = false;
				map[y][x].has_transition = false;
				map[y][x].item = generateItem();
			}else if((rand() % 40) == 0 && enemies > 0){
				enemies--;
				Monster new_enemy = selectMonster(y, x);
				new_enemy.pos.y = y;
				new_enemy.pos.x = x;
				new_enemy.wait_time = 0;
				map[y][x].monster = &new_enemy;
				//map[y][x].ch = map[y][x].monster->ch;
				map[y][x].has_item = false;
				map[y][x].has_monster = true;
				map[y][x].has_transition = false;
				map[y][x].ch = '.';
				monsters[monster_index] = new_enemy;
				monster_index++;
			//}else if((rand() % 60) == 50 && doorway > 0){
			//	doorway = rand()%3==2?0:1;
			//	map[y][x].ch = 'v';
			//	map[y][x].has_item = false;
			//	map[y][x].has_monster = false;
			//	map[y][x].has_transition = true;
			}else{
				map[y][x].ch = '.';
				map[y][x].has_monster = false;
				map[y][x].has_item = false;
				map[y][x].has_transition = false;
			}
			map[y][x].walkable = true;
			map[y][x].transparent = true;
		}
	}
}

void connectRooms(Position roomOneCenter, Position roomTwoCenter){
	Position temp;
	temp.x = roomOneCenter.x;
	temp.y = roomOneCenter.y;

	while(true){
		if(abs((temp.x - 1) - roomTwoCenter.x) < abs(temp.x - roomTwoCenter.x)){
			temp.x--;
		}else if(abs((temp.x + 1) - roomTwoCenter.x) < abs(temp.x - roomTwoCenter.x)){
			temp.x++;
		}else if(abs((temp.y - 1) - roomTwoCenter.y) < abs(temp.y - roomTwoCenter.y)){
			temp.y--;
		}else if(abs((temp.y + 1) - roomTwoCenter.y) < abs(temp.y - roomTwoCenter.y)){
			temp.y++;
		}else{
			break;
		}
		map[temp.y][temp.x].ch = '.';
		map[temp.y][temp.x].walkable = true;
		map[temp.y][temp.x].transparent = true;
		if(map[temp.y][temp.x].has_transition){
			map[temp.y][temp.x].has_transition = false;
			map[temp.y][temp.x].ch = '.';
			//	map[temp.y][temp.x].ch = 'v';
		}else if(map[temp.y][temp.x].has_item){
			map[temp.y][temp.x].ch = 'i';
		}
	}
}

Tile** createMapTiles(){
	Tile** tiles = calloc(MAP_HEIGHT, sizeof(Tile*));

	for (int y = 0; y<MAP_HEIGHT; y++){
		tiles[y] = calloc(MAP_WIDTH, sizeof(Tile));
		for (int x=0; x < MAP_WIDTH; x++){
			tiles[y][x].ch = '#';
			tiles[y][x].color = COLOR_PAIR(VISIBLE_COLOR);
			tiles[y][x].walkable=false;
			tiles[y][x].transparent = false;
			tiles[y][x].visible = false;
			tiles[y][x].seen = false;
			tiles[y][x].has_transition = false;
		}
	}
	return tiles;
}

void drawMap(){
		for(int y = 0; y < MAP_HEIGHT; y++){
			for(int x = 0; x < MAP_WIDTH; x++){
				if(map[y][x].visible){
					if(map[y][x].has_monster){
								mvaddch(y, x, map[y][x].monster->ch | map[y][x].color);
					}else{
						mvaddch(y,x,map[y][x].ch | map[y][x].color);
					}
				} else if(map[y][x].seen){
					if(map[y][x].ch != '#'){
						mvaddch(y,x,'.' | COLOR_PAIR(SEEN_COLOR));
					}else{
						mvaddch(y,x,'#' | COLOR_PAIR(SEEN_COLOR));
					}
				} else {
					mvaddch(y,x,' ');
				}
			}
	}
}

Position setupRooms(){
	srand(time(NULL));
	int y, x, height, width, n_rooms;
	n_rooms = (rand() % 11) + 5;
	Room *rooms = calloc(n_rooms, sizeof(Room));
	for(int i = 0; i < n_rooms; i++){
		y = (rand() % (MAP_HEIGHT - 12)) + 2;
		x = (rand() % (MAP_WIDTH - 22)) + 2;
		height = (rand() % 7) + 3;
		width = (rand() % 15) + 5;
		rooms[i] = createRoom(y, x, height, width);
		addRoom(rooms[i]);
		if(i > 0){
			connectRooms(rooms[i-1].center, rooms[i].center);
		}
	}
	map[rooms[n_rooms-1].center.y][rooms[n_rooms-1].center.x].has_transition = true;
	map[rooms[n_rooms-1].center.y][rooms[n_rooms-1].center.x].ch = 'v';
	map[rooms[n_rooms-1].center.y][rooms[n_rooms-1].center.x].walkable = true;
	return rooms[0].center;
}

Monster selectMonster(int y, int x){
	Monster * new_monster = calloc(1, sizeof(Monster));
	int spawn = rand()%100;
		if(spawn < 3){
			strcpy(new_monster->name, "dragon");
			new_monster->num_of_attacks = 2;
			Attack attack1;
			strcpy(attack1.name, "Fire Breath");
			attack1.damage = 50;
			attack1.mana_cost = 0;
			attack1.attack_element = FIRE;
			attack1.accuracy = 60;
			attack1.status = BURNING;
			attack1.status_chance = 100;
			Attack attack2;
			strcpy(attack2.name, "Slash");
			attack2.damage = 25;
			attack2.mana_cost = 0;
			attack2.attack_element = NONE;
			attack2.accuracy = 90;
			attack2.status = BLEEDING;
			attack2.status_chance = 30;
			new_monster->attacks[0] = attack1;
			new_monster->attacks[1] = attack2;
			new_monster->ch = 'D';
			new_monster->max_health = 500;
			new_monster->health = 500;
			new_monster->mana = 200;
			new_monster->exp = 100;
			new_monster->dead = false;
			new_monster->status = NO_STATUS;
			new_monster->status_timer = 0;
			new_monster->resistance = FIRE;
			new_monster->weakness = ICE;
		}else if(spawn < 15){
			strcpy(new_monster->name, "gargoyle");
			new_monster->ch = 'G';
			new_monster->max_health = 200;
			new_monster->health = 200;
			new_monster->mana = 0;
			Attack attack1;
			strcpy(attack1.name, "Slash");
			attack1.damage = 10;
			attack1.mana_cost = 0;
			attack1.attack_element = NONE;
			attack1.accuracy = 90;
			attack1.status = BLEEDING;
			attack1.status_chance = 20;
			Attack attack2;
			strcpy(attack2.name, "Stone Throw");
			attack2.damage = 5;
			attack2.mana_cost = 0;
			attack2.attack_element = NONE;
			attack2.accuracy = 50;
			attack2.status = NO_STATUS;
			attack2.status_chance = 0;
			new_monster->num_of_attacks = 2;
			new_monster->attacks[0] = attack1;
			new_monster->attacks[1] = attack2;
			new_monster->exp = 60;
			new_monster->dead = false;
			new_monster->status = NO_STATUS;
			new_monster->status_timer = 0;
			new_monster->resistance = FIRE;
			new_monster->weakness = NONE;
		}else if(spawn < 30){
			strcpy(new_monster->name, "goblin");
			new_monster->ch = 'g';
			new_monster->max_health = 10;
			new_monster->health = 10;
			new_monster->mana = 0;
			new_monster->num_of_attacks = 1;
			Attack attack1;
			strcpy(attack1.name, "Stab");
			attack1.damage = 3;
			attack1.mana_cost = 0;
			attack1.attack_element = NONE;
			attack1.accuracy = 70;
			attack1.status = BLEEDING;
			attack1.status_chance = 10;
			new_monster->attacks[0] = attack1;
			new_monster->exp = 2;
			new_monster->dead = false;
			new_monster->status = NO_STATUS;
			new_monster->status_timer = 0;
			new_monster->resistance = NONE;
			new_monster->weakness = NONE;
		}else if(spawn < 40){
			strcpy(new_monster->name, "lamia");
			new_monster->ch = 'l';
			new_monster->max_health = 60;
			new_monster->health = 60;
			new_monster->mana = 100;
			new_monster->num_of_attacks = 3;
			Attack attack1;
			strcpy(attack1.name, "Constrict");
			attack1.damage = 20;
			attack1.attack_element = NONE;
			attack1.mana_cost = 0;
			attack1.accuracy = 80;
			attack1.status = NO_STATUS;
			attack1.status_chance = 0;
			Attack attack2;
			strcpy(attack2.name, "Poison");
			attack2.damage = 0;
			attack2.attack_element = NONE;
			attack2.mana_cost = 0;
			attack2.accuracy = 90;
			attack2.status = POISONED;
			attack2.status_chance = 100;
			Attack attack3;
			strcpy(attack3.name, "Tail Whip");
			attack3.damage = 8;
			attack3.mana_cost = 0;
			attack3.attack_element = NONE;
			attack3.accuracy = 100;
			attack3.status = NO_STATUS;
			attack3.status_chance = 0;
			new_monster->attacks[0] = attack1;
			new_monster->attacks[1] = attack2;
			new_monster->attacks[2] = attack3;
			new_monster->exp = 12;
			new_monster->dead = false;
			new_monster->status = NO_STATUS;
			new_monster->status_timer = 0;
			new_monster->resistance = NONE;
			new_monster->weakness = ICE;
		}else if(spawn < 50){
			strcpy(new_monster->name, "orc");
			new_monster->ch = 'o';
			new_monster->max_health = 120;
			new_monster->health = 120;
			new_monster->mana = 0;
			new_monster->num_of_attacks = 2;
			Attack attack1;
			strcpy(attack1.name, "Charge");
			attack1.damage = 10;
			attack1.attack_element = NONE;
			attack1.accuracy = 80;
			attack1.mana_cost = 0;
			attack1.status = NO_STATUS;
			attack1.status_chance = 0;
			Attack attack2;
			strcpy(attack2.name, "Beserk");
			attack2.damage = 30;
			attack2.attack_element = NONE;
			attack2.mana_cost = 0;
			attack2.accuracy = 50;
			attack2.status = NO_STATUS;
			attack2.status_chance = 0;
			new_monster->attacks[0] = attack1;
			new_monster->attacks[1] = attack2;
			new_monster->exp = 20;
			new_monster->dead = false;
			new_monster->status = NO_STATUS;
			new_monster->status_timer = 0;
			new_monster->resistance = NONE;
			new_monster->weakness = NONE;
		}else if(spawn < 60){strcpy(new_monster->name, "rogue");
			new_monster->ch = 'r';
			new_monster->max_health = 40;
			new_monster->health = 40;
			new_monster->mana = 0;
			new_monster->num_of_attacks = 2;
			Attack attack1;
			strcpy(attack1.name, "Stab");
			attack1.damage = 10;
			attack1.attack_element = NONE;
			attack1.mana_cost = 0;
			attack1.accuracy = 90;
			attack1.status = NO_STATUS;
			attack1.status_chance = 0;
			Attack attack2;
			strcpy(attack2.name, "Thrown Knives");
			attack2.damage = 12;
			attack2.accuracy = 90;
			attack2.status = NO_STATUS;
			attack2.status_chance = 0;
			attack2.mana_cost = 0;
			new_monster->attacks[0] = attack1;
			new_monster->attacks[1] = attack2;
			new_monster->exp = 10;
			new_monster->dead = false;
			new_monster->status = NO_STATUS;
			new_monster->status_timer = 0;
			new_monster->resistance = NONE;
			new_monster->weakness = NONE;
		}else if(spawn < 90){
			strcpy(new_monster->name, "spider");
			new_monster->ch = 's';
			new_monster->max_health = 20;
			new_monster->health = 20;
			new_monster->mana = 0;
			new_monster->num_of_attacks = 1;
			Attack attack1;
			strcpy(attack1.name, "Bite");
			attack1.damage = 8;
			attack1.attack_element = NONE;
			attack1.mana_cost = 0;
			attack1.accuracy = 90;
			attack1.status = POISONED;
			attack1.status_chance = 40;
			new_monster->attacks[0] = attack1;
			new_monster->exp = 5;
			new_monster->dead = false;
			new_monster->status = NO_STATUS;
			new_monster->status_timer = 0;
			new_monster->resistance = NONE;
			new_monster->weakness = FIRE;
		}else if(spawn < 100){
			strcpy(new_monster->name, "succubus");
			new_monster->ch = 'S';
			new_monster->max_health = 90;
			new_monster->health = 90;
			new_monster->mana = 180;
			new_monster->num_of_attacks = 2;
			Attack attack1;
			strcpy(attack1.name, "Scratch");
			attack1.damage = 12;
			attack1.attack_element = NONE;
			attack1.mana_cost = 0;
			attack1.accuracy = 90;
			attack1.status = BLEEDING;
			attack1.status_chance = 80;
			Attack attack2;
			strcpy(attack2.name, "Fireball");
			attack2.mana_cost = 30;
			attack2.damage = 20;
			attack2.accuracy = 80;
			attack2.attack_element = FIRE;
			attack2.status = BURNING;
			attack2.status_chance = 100;
			new_monster->attacks[0] = attack1;
			new_monster->attacks[1] = attack2;
			new_monster->exp = 30;
			new_monster->dead = false;
			new_monster->status = NO_STATUS;
			new_monster->status_timer = 0;
			new_monster->resistance = FIRE;
			new_monster->weakness = HOLY;
		}
	return *new_monster;
}

Item * generateItem(){

	switch(rand()%5){
		case 0:
			return generateWeapon();
			break;
		case 1:
			return generateArmor();
			break;
		case 2:
			return generateSpell();
			break;
		case 3:
		case 4:
			return generateConsumable();
			break;
	}
}

Item * generateWeapon(){
			Item * new_item = calloc(1, sizeof(Item));
			new_item->type = WEAPON;
			char weapon_name[100];
			char material[20];
			char weapon_type[25];
			char enchantment_name[15];
			char description[255];
			float base_damage;
			float mat_multi;
			switch(rand()%15){
				case 0:
					strcpy(weapon_type, "Dagger");
					new_item->weapon.type = DAGGER;
					new_item->weapon.is_two_handed = false;
					base_damage = 1.2;
					break;
				case 1:
					strcpy(weapon_type, "Broad Sword");
					new_item->weapon.type = BROAD_SWORD;
					new_item->weapon.is_two_handed = false;
					base_damage = 1.7;
					break;
				case 2:
					strcpy(weapon_type, "Long Sword");
					new_item->weapon.type = LONG_SWORD;
					new_item->weapon.is_two_handed = false;
					base_damage = 1.5;
					break;
				case 3:
					strcpy(weapon_type, "Scimitar");
					new_item->weapon.type = SCIMITAR;
					new_item->weapon.is_two_handed = false;
					base_damage = 1.6;
					break;
				case 4:
					strcpy(weapon_type, "Katana");
					new_item->weapon.type = KATANA;
					new_item->weapon.is_two_handed = false;
					base_damage = 2.0;
					break;
				case 5:
					strcpy(weapon_type, "War Axe");
					new_item->weapon.type = WAR_AXE;
					new_item->weapon.is_two_handed = false;
					base_damage = 2.4;
					break;
				case 6:
					strcpy(weapon_type, "Battle Axe");
					new_item->weapon.type = BATTLE_AXE;
					new_item->weapon.is_two_handed = true;
					base_damage = 3.2;
					break;
				case 7:
					strcpy(weapon_type, "War Hammer");
					new_item->weapon.type = WAR_HAMMER;
					new_item->weapon.is_two_handed = false;
					base_damage = 2.6;
					break;
				case 8:
					strcpy(weapon_type, "Spear");
					new_item->weapon.type = SPEAR;
					new_item->weapon.is_two_handed = true;
					base_damage = 2.3;
					break;
				case 9:
					strcpy(weapon_type, "Halberd");
					new_item->weapon.type = HALBERD;
					new_item->weapon.is_two_handed = true;
					base_damage = 3.0;
					break;
				case 10:
					strcpy(weapon_type, "Scythe");
					new_item->weapon.type = SCYTHE;
					new_item->weapon.is_two_handed = true;
					base_damage = 2.7;
					break;
				case 11:
					strcpy(weapon_type, "Bow");
					new_item->weapon.type = BOW;
					new_item->weapon.is_two_handed = true;
					base_damage = 2.2;
					break;
				case 12:
					strcpy(weapon_type, "Throwing Knives");
					new_item->weapon.type = THROWING_KNIVES;
					new_item->weapon.is_two_handed = false;
					base_damage = 2.8;
					new_item->weapon.count = (rand()%20)+10;
					break;
				case 13:
					strcpy(weapon_type, "Shield");
					new_item->weapon.type = SHIELD;
					new_item->weapon.is_two_handed = false;
					base_damage = 2;
					break;
				case 14:
					strcpy(weapon_type, "Arrows");
					new_item->weapon.type = ARROWS;
					new_item->weapon.is_two_handed = false;
					base_damage = 0.5;
					new_item->weapon.count = (rand()%60)+40;
					break;
			}
			int weapon_material_chance = roundf((rand()%(70-current_player->floor < 1?1:70-current_player->floor))+current_player->floor);
			if(weapon_material_chance > 100){
				weapon_material_chance = 100;
			}
			if(weapon_material_chance < 30){
				strcpy(material, "Bronze");
				new_item->weapon.material = BRONZE;
				mat_multi = 1;
			}else if(weapon_material_chance < 60){
				strcpy(material, "Iron");
				new_item->weapon.material = IRON;
				mat_multi = 1.2;
			}else if(weapon_material_chance < 80){
				strcpy(material, "Steel");
				new_item->weapon.material = STEEL;
				mat_multi = 1.5;
			}else if(weapon_material_chance < 90){
				strcpy(material, "Silver");
				new_item->weapon.material = SILVER;
				mat_multi =1.1;
			}else if(weapon_material_chance < 97){
				strcpy(material, "Mithril");
				new_item->weapon.material = MITHRIL;
				mat_multi = 1.8;
			}else{
				strcpy(material, "Adamantite");
				new_item->weapon.material = ADAMANTITE;
				mat_multi = 2.0;
			}
			int enchantment_chance = rand()%100;
			if(enchantment_chance > 10 && enchantment_chance < 13 && new_item->weapon.type != SHIELD && new_item->weapon.type != ARROWS){
				strcpy(enchantment_name, "Burning");
				new_item->weapon.element = FIRE;
			} else if(enchantment_chance > 30 && enchantment_chance < 33 && new_item->weapon.type != SHIELD && new_item->weapon.type != ARROWS){
				strcpy(enchantment_name, "Arcing");
				new_item->weapon.element = SHOCK;
			} else if(enchantment_chance > 50 && enchantment_chance < 53 && new_item->weapon.type != SHIELD && new_item->weapon.type != ARROWS){
				strcpy(enchantment_name, "Freezing");
				new_item->weapon.element = ICE;
			} else if(enchantment_chance > 70 && enchantment_chance < 73 && new_item->weapon.type != SHIELD && new_item->weapon.type != ARROWS){
				strcpy(enchantment_name, "Draining");
				new_item->weapon.element = DRAIN;
			} else if(enchantment_chance > 90 && enchantment_chance < 93 && new_item->weapon.type != SHIELD && new_item->weapon.type != ARROWS){
				strcpy(enchantment_name, "Blessed");
				new_item->weapon.element = HOLY;
			} else{
				new_item->weapon.element = NONE;
			}
			if(new_item->weapon.element == NONE){
				new_item->weapon.damage = (base_damage * mat_multi);
				sprintf(weapon_name, "%s %s", material, weapon_type);
				sprintf(description, "A %s made of %s", weapon_type, material);
			}else{
				new_item->weapon.damage = (base_damage * mat_multi)+2.0;
				sprintf(weapon_name, "%s %s %s", enchantment_name, material, weapon_type);
				sprintf(description, "A %s made of %s, it's also enchanted with %s", weapon_type, material, enchantment_name);
			}
			strcpy(new_item->name, weapon_name);
			strcpy(new_item->weapon.name, weapon_name);
			strcpy(new_item->description, description);
			strcpy(new_item->weapon.description, description);

	new_item->armor.is_equipped = false;
	new_item->weapon.is_equipped = false;
	return new_item;
}

Item * generateArmor(){
			Item * new_item = calloc(1, sizeof(Item));
			new_item->type = ARMOR;
			char armor_name[100];
			char armor_material[20];
			char armor_type[25];
			char armor_description[255];
			int protection;
			int armor_material_chance = roundf((rand()%(70-current_player->floor < 1?1:70-current_player->floor))+current_player->floor);
			if(armor_material_chance > 100){
				armor_material_chance = 100;
			}
			if(armor_material_chance < 40){
				strcpy(armor_material, "Bronze");
				new_item->armor.material = BRONZE;
				new_item->armor.protection = 1;
			}else if(armor_material_chance < 70){
				strcpy(armor_material, "Iron");
				new_item->armor.material = IRON;
				new_item->armor.protection = 2;
			}else if(armor_material_chance < 90){
				strcpy(armor_material, "Steel");
				new_item->armor.material = STEEL;
				new_item->armor.protection = 3;
			}else if(armor_material_chance < 97){
				strcpy(armor_material, "Mithril");
				new_item->armor.material = MITHRIL;
				new_item->armor.protection = 4;
			}else{
				strcpy(armor_material, "Adamantite");
				new_item->armor.material = ADAMANTITE;
				new_item->armor.protection = 5;
			}
			switch(rand()%5){
				case 0:
					new_item->armor.type = HEAD;
					strcpy(armor_type, "Helmet");
					break;
				case 1:
					new_item->armor.type = BODY;
					strcpy(armor_type, "Chestpiece");
					break;
				case 2:
					new_item->armor.type = HANDS;
					strcpy(armor_type, "Gauntlets");
					break;
				case 3:
					new_item->armor.type = LEGS;
					strcpy(armor_type, "Greaves");
					break;
				case 4:
					new_item->armor.type = FEET;
					strcpy(armor_type, "Boots");
					break;
			}
			sprintf(armor_name, "%s %s", armor_material, armor_type);
			sprintf(armor_description, "%s made of %s", armor_type, armor_material);
			strcpy(new_item->name, armor_name);
			strcpy(new_item->armor.name, armor_name);
			strcpy(new_item->description, armor_description);
			strcpy(new_item->armor.description, armor_description);
	new_item->armor.is_equipped = false;
	new_item->weapon.is_equipped = false;
	return new_item;
}

Item * generateSpell(){
			Item * new_item = calloc(1, sizeof(Item));
			new_item->type = SPELL;
			char spell_name[50];
			char spell_description[255];
			int pre_tier = roundf((rand()%(70-current_player->floor < 1?1:70-current_player->floor))+current_player->floor);
			if(pre_tier > 100){
				pre_tier = 100;
			}
			int tier = 0;
			if(pre_tier<50){
				tier = 1;
			}else if(pre_tier<85){
				tier = 2;
			}else{
				tier = 3;
			}
			int damage = (rand()%5)+(tier*7);
			int mana_cost = ((rand()%(tier*3))+(tier*6));
			ELEMENT element = (rand()%5)+1;
			int archtype = rand()%100;
			if(archtype < 70){
				int type = rand()%5;
				switch(type){
					case 0:
						new_item->spell.element = FIRE;
						new_item->spell.is_offensive = true;
						new_item->spell.is_usable = false;
						new_item->spell.type = DAMAGE;
						if(tier == 1){
							sprintf(spell_name, "Fire");
							sprintf(spell_description, "A weak fire spell");
						}else if(tier == 2){
							sprintf(spell_name, "Fira");
							sprintf(spell_description, "A moderate fire spell");
						}else{
							sprintf(spell_name, "Firaga");
							sprintf(spell_description, "A powerful fire spell");
						}
						break;
					case 1:
						new_item->spell.element = SHOCK;
						new_item->spell.is_offensive = true;
						new_item->spell.is_usable = false;
						new_item->spell.type = DAMAGE;
						if(tier == 1){
							sprintf(spell_name, "Thunder");
							sprintf(spell_description, "A weak lightning spell");
						}else if(tier == 2){
							sprintf(spell_name, "Thundara");
							sprintf(spell_description, "A moderate lightning spell");
						}else{
							sprintf(spell_name, "Thundaga");
							sprintf(spell_description, "A powerful lightning spell");
						}
						break;
					case 2:
						new_item->spell.element = ICE;
						new_item->spell.is_offensive = true;
						new_item->spell.is_usable = false;
						new_item->spell.type = DAMAGE;
						if(tier == 1){
							sprintf(spell_name, "Blizzard");
							sprintf(spell_description, "A weak ice spell");
						}else if(tier == 2){
							sprintf(spell_name, "Blizzara");
							sprintf(spell_description, "A moderate ice spell");
						}else{
							sprintf(spell_name, "Blizzaga");
							sprintf(spell_description, "A powerful ice spell");
						}
						break;
					case 3:
						new_item->spell.element = DRAIN;
						new_item->spell.is_offensive = true;
						new_item->spell.is_usable = false;
						new_item->spell.type = DAMAGE;
						if(tier == 1){
							sprintf(spell_name, "Drain");
							sprintf(spell_description, "A weak drain spell");
						}else if(tier == 2){
							sprintf(spell_name, "Drainara");
							sprintf(spell_description, "A moderate drain spell");
						}else{
							sprintf(spell_name, "Draindaga");
							sprintf(spell_description, "A powerful drain spell");
						}
						break;
					case 4:
						new_item->spell.element = HOLY;
						new_item->spell.is_offensive = true;
						new_item->spell.is_usable = false;
						new_item->spell.type = DAMAGE;
						if(tier == 1){
							sprintf(spell_name, "Holy");
							sprintf(spell_description, "A weak holy spell");
						}else if(tier == 2){
							sprintf(spell_name, "Holara");
							sprintf(spell_description, "A moderate holy spell");
						}else{
							sprintf(spell_name, "Holdaga");
							sprintf(spell_description, "A powerful holy spell");
						}
						break;
					}
			}else if(archtype < 90){
				switch(rand()%2){
					case 0:
						new_item->spell.is_offensive = false;
						new_item->spell.is_usable = true;
						new_item->spell.element = NONE;
						new_item->spell.type = HEALING;
						sprintf(spell_name, "Heal");
						if(tier == 1){
							sprintf(spell_description, "A weak healing spell");
						}else if(tier == 2){
							sprintf(spell_description, "A moderate healing spell");
						}else{
							sprintf(spell_description, "A powerful healing spell");
						}
						break;
					case 1:
						new_item->spell.type = PROTECT;
						new_item->spell.is_offensive = false;
						new_item->spell.is_usable = false;
						new_item->spell.element = NONE;
						sprintf(spell_name, "Protect");
						if(tier == 1){
							sprintf(spell_description, "A weak protection spell");
						}else if(tier == 2){
							sprintf(spell_description, "A moderate protecton spell");
						}else{
							sprintf(spell_description, "A powerful protection spell");
						}
						break;

					default:
					strcpy(spell_name, "element default");
					break;
				}
			}else{
				switch(rand()%2){
					case 0:
						new_item->spell.type = INFLICT_STATUS;
						new_item->spell.element = NONE;
						new_item->spell.is_offensive = true;
						new_item->spell.is_usable = false;
						new_item->spell.status = INSANITY;
						sprintf(spell_name, "Insanity");
						if(tier == 1){
							sprintf(spell_description, "A weak spell that inflicts Insanity");
						}else if(tier == 2){
							sprintf(spell_description, "A moderate spell that inflicts Insanity");
						}else{
							sprintf(spell_description, "A powerful spell that inflicts Insantiy");
						}
						break;
					case 1:
						new_item->spell.type = INFLICT_STATUS;
						new_item->spell.element = NONE;
						new_item->spell.is_offensive = true;
						new_item->spell.is_usable = false;
						new_item->spell.status = SILENCED;
						sprintf(spell_name, "Silence");
						if(tier == 1){
							sprintf(spell_description, "A weak spell that inflicts Silence");
						}else if(tier == 2){
							sprintf(spell_description, "A moderate spell that inflicts Silence");
						}else{
							sprintf(spell_description, "A powerful spell that inflicts Silence");
						}
						new_item->spell.num_of_effects = 1;
						break;
				}
			}
			strcpy(new_item->name, spell_name);
			strcpy(new_item->description, spell_description);
			strcpy(new_item->spell.name, spell_name);
			strcpy(new_item->spell.description, spell_description);
			new_item->spell.mana_cost = mana_cost;
			new_item->spell.damage = new_item->spell.element == DRAIN?damage/2:damage;
			new_item->spell.tier = tier;
	new_item->armor.is_equipped = false;
	new_item->weapon.is_equipped = false;
	return new_item;
}

Item * generateConsumable(){
	Item * new_item = calloc(1, sizeof(Item));
	new_item->type = CONSUMABLE;
	new_item->consumable.type = rand()%4;
	int pre_tier = rand()%100;
	int tier = 0;
	if(pre_tier<50){
		tier = 1;
	}else if(pre_tier<85){
		tier = 2;
	}else{
		tier = 3;
	}
	switch(new_item->consumable.type){
		case SCROLL:
			Spell scroll_spell = generateSpell()->spell;
			sprintf(new_item->name, "Scroll of %s", scroll_spell.name);
			strcpy(new_item->description, scroll_spell.description);
			new_item->consumable.scroll_spell = scroll_spell;
			new_item->consumable.scroll_spell.mana_cost = 0;
			break;
		case HEALTH_POTION:
			sprintf(new_item->name, "Potion of health");
			sprintf(new_item->description, "A potion that restores %d health", tier*10);
			new_item->consumable.tier = tier;
			break;
		case MANA_POTION:
			sprintf(new_item->name, "Potion of mana");
			sprintf(new_item->description, "A potion that restores %d mana", tier*10);
			new_item->consumable.tier = tier;
			break;
		case BOOK_OF_KNOWLEDGE:
			sprintf(new_item->name, "Tome of knowledge");
			sprintf(new_item->description, "An ancient tome filled with knowledge, grants one skill point when read");
	}
	return new_item;
}

void moveMonsters(){
			for(int i = 0; i < monster_index; i++){
					if(monsters[i].wait_time == 0){
						if(!monsters[i].dead){
							int rand_x = rand()%3;
							int rand_y = rand()%3;
							int temp_x = monsters[i].pos.x;
							int temp_y = monsters[i].pos.y;
							if(map[monsters[i].pos.y][monsters[i].pos.x].visible){
								if((rand()%10) < 8){
									if(rand()%2==0){
										temp_x = current_player->pos.x > monsters[i].pos.x? temp_x + 1 : temp_x;
										temp_x = current_player->pos.x < monsters[i].pos.x? temp_x - 1 : temp_x;
									}
									if(rand()%2==0){
										temp_y = current_player->pos.y > monsters[i].pos.y? temp_y + 1 : temp_y;
										temp_y = current_player->pos.y < monsters[i].pos.y? temp_y - 1 : temp_y;
									}
								}
							}else{
							switch(rand_x){
								case 0:
									temp_x++;
									break;
								case 1:
									break;
								case 2:
									temp_x--;
									break;
							}
							switch(rand_y){
								case 0:
									temp_y++;
									break;
								case 1:
									break;
								case 2:
									temp_y--;
									break;
							}
							}
							if(map[temp_y][temp_x].walkable){
								map[temp_y][temp_x].monster = &monsters[i];
								map[monsters[i].pos.y][monsters[i].pos.x].has_monster = false;
								map[temp_y][temp_x].has_monster = true;
								monsters[i].pos.y = temp_y;
								monsters[i].pos.x = temp_x;
								if(current_player->pos.x == monsters[i].pos.x && monsters[i].pos.y == current_player->pos.y){
									battleLoop(&monsters[i], current_player);
								}
							}
						}
					}else{
						monsters[i].wait_time--;
					}
			}
}

Position generateMap(){
	current_player->floor++;
	monster_index = 0;
	doorway = 1;
	Position pos;
	map = createMapTiles();
	pos = setupRooms();
	moveMonsters();
	drawMap();
	return pos;
}

void regenerateMap(bool has_moved){
	drawMap();
	move(0,0);
	printw("Floor: %d", current_player->floor);
}
