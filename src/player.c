#include "player.h"
#include <ncurses.h>
#include <stdlib.h>
#include "generation.h"
#include "world.h"
#include <math.h>

Player *createPlayer(char name[], Stats *stats){
	Player *player = calloc(1, sizeof(Player));
	strcpy(player->name, name);
	player->stats = *stats;
	player->max_health = (10 + (player->stats.constitution) * 2);
	player->max_mana = (player->stats.wisdom * 4);
	player->health = player->max_health;
	player->mana = player->max_mana;
	player->lvl = 1;
	player->exp = 0;
	player->color = COLOR_PAIR(VISIBLE_COLOR);
	player->ch = '@';

	Item blank_item;

	blank_item.name[0] = '\0';
	blank_item.armor.name[0] = '\0';
	blank_item.weapon.name[0] = '\0';
	blank_item.weapon.damage = 0;
	blank_item.armor.protection = 0;
	blank_item.weapon.count = 0;
	blank_item.weapon.is_two_handed = false;


	player->main_hand = blank_item;
	player->off_hand = blank_item;
	player->arrows = blank_item;
	player->head = blank_item;
	player->body = blank_item;
	player->legs = blank_item;
	player->hands = blank_item;
	player->feet = blank_item;

	player->floor = 0;

	return player;
}

int getDamage(Weapon * weapon, Player * player){
	int damage;
	if(weapon->type == DAGGER || weapon->type == KATANA || weapon->type == BOW || weapon->type == THROWING_KNIVES || weapon->type == ARROWS || weapon->type == SCIMITAR || weapon->type == SPEAR || weapon->type == SCYTHE){
			damage = weapon->damage + roundf((weapon->damage/4) * ((player->stats.dexterity)));

//	}else if(weapon->type == SWORD){
			//damage = weapon->damage + roundf((weapon->damage/2) * (1+(player->stats.dexterity/3)));
			//damage = roundf(weapon->damage * (1+((player->stats.dexterity+player->stats.strength)/2)));
	}else{
			damage = weapon->damage + roundf((weapon->damage/4) * ((player->stats.strength)));
			//damage = roundf(weapon->damage * (1+(player->stats.strength/3)));
	}
	return damage<1?1:damage;
}

int getSpellDamage(Spell * spell, Player * player){
	return spell->damage + roundf((spell->damage/6) * ((player->stats.intelligence)));
	//return roundf(spell->damage * (1+(player->stats.intelligence/3)));
}
