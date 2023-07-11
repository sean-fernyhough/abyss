#ifndef WORLD_H
#define WORLD_H

#include <stdbool.h>

typedef enum {
	WEAPON,
	ARMOR,
	SPELL,
	CONSUMABLE,
	TOOL
} ITEM_TYPE;

typedef enum {
	SCROLL,
	HEALTH_POTION,
	MANA_POTION,
	BOOK_OF_KNOWLEDGE
} CONSUMABLE_TYPE;

typedef enum {
	NO_STATUS,
	POISONED,
	SILENCED,
	PARALYSED,
	BLEEDING,
	BURNING,
	FROZEN,
	INSANITY
} STATUS_EFFECTS;

typedef enum {
	BRONZE,
	COPPER,
	IRON,
	STEEL,
	SILVER,
	MITHRIL,
	ADAMANTITE
} MATERIAL;

typedef enum {
	DAGGER,
	BROAD_SWORD,
	LONG_SWORD,
	SCIMITAR,
	KATANA,
	WAR_AXE,
	BATTLE_AXE,
	WAR_HAMMER,
	SPEAR,
	HALBERD,
	SCYTHE,
	BOW,
	THROWING_KNIVES,
	SHIELD,
	ARROWS
} WEAPON_TYPE;

typedef enum {
	HEAD,
	BODY,
	HANDS,
	LEGS,
	FEET
} ARMOR_TYPE;

typedef enum {
	NONE,
	FIRE,
	SHOCK,
	ICE,
	DRAIN,
	HOLY
} ELEMENT;


typedef enum {
	HEALING,
	PROTECT,
	INFLICT_STATUS,
	DAMAGE
} SPELL_TYPE;


typedef enum {
	PHYSICAL,
	MAGIC
} DAMAGE_TYPE;

typedef struct {
	char name[50];
	int damage;
	int mana_cost;
	char description[255];
	bool is_offensive;
	int tier;
	ELEMENT element;
	int num_of_effects;
	STATUS_EFFECTS status;
	bool is_usable;
	SPELL_TYPE type;
} Spell;

typedef struct {
	char name[50];
	float damage;
	char description[255];
	int count;
	bool is_equipped;
	bool is_two_handed;
	WEAPON_TYPE type;
	MATERIAL material;
	ELEMENT element;
} Weapon;

typedef struct {
	char name[50];
	int protection;
	char description[255];
	bool is_equipped;
	ARMOR_TYPE type;
	MATERIAL material;
} Armor;

typedef struct {
	char name[50];
	char description[255];
	CONSUMABLE_TYPE type;
	int tier;
	Spell scroll_spell;
} Consumable;

typedef struct {
	char name[100];
	ITEM_TYPE type;
	Weapon weapon;
	Armor armor;
	Spell spell;
	Consumable consumable;
	char description[255];
} Item;

typedef struct {
	int x;
	int y;
} Position;

typedef struct {
	char name[30];
	int damage;
	int mana_cost;
	ELEMENT attack_element;
	int accuracy;
	STATUS_EFFECTS status;
	int status_chance;
} Attack;

typedef struct {
	char ch;
	char name[30];
	int num_of_attacks;
	Attack attacks[10];
	int damage;
	int health;
	int mana;
	Position pos;
	int exp;
	bool dead;
	int wait_time;
	int status_timer;
	int max_health;
	STATUS_EFFECTS status;
	ELEMENT resistance;
	ELEMENT weakness;
} Monster;

typedef struct {
	int constitution;
	int strength;
	int dexterity;
	int intelligence;
	int endurence;
	int wisdom;
} Stats;

typedef struct {
	char name[100];
	int max_health;
	int health;
	int lvl;
	int exp;
	int mana;
	int max_mana;
	Position pos;
	Item inventory[18];
	Spell spellbook[18];
	int color;
	int ch;
	Item head;
	Item body;
	Item hands;
	Item legs;
	Item feet;
	Stats stats;
	Item off_hand;
	Item main_hand;
	Item arrows;
	int skill_points;
	int status_timer;
	STATUS_EFFECTS status;
	int current_step;
	int floor;
	int spell_protection;
} Player;

#endif
