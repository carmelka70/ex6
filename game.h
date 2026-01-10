#ifndef GAME_H
#define GAME_H

#include "bst.h"

typedef enum { ARMOR, SWORD } ItemType;
typedef enum { PHANTOM, SPIDER, DEMON, GOLEM, COBRA } MonsterType;

typedef struct Item {
    char* name;
    ItemType type;
    int value;
} Item;

typedef struct Monster {
    char* name;
    MonsterType type;
    int hp;
    int maxHp;
    int attack;
} Monster;

typedef struct Room {
    int id;
    int x, y;
    int visited;
    Monster* monster;
    Item* item;
    struct Room* next;
} Room;

typedef struct Player {
    int hp;
    int maxHp;
    int baseAttack;
    BST* bag;
    BST* defeatedMonsters;
    Room* currentRoom;
} Player;

typedef struct {
    Room* rooms;
    Player* player;
    int roomCount;
    int configMaxHp;
    int configBaseAttack;
} GameState;

// Monster functions
void freeMonster(void* data);
cmpValue compareMonsters(void* a, void* b);
void printMonster(void* data);

// Item functions
void freeItem(void* data);
cmpValue compareItems(void* a, void* b);
void printItem(void* data);

// Game functions
void addRoom(GameState* g);
Room* findRoomById(GameState *g, int id);
Room* findRoomByPos(GameState *g, int x, int y);
void initPlayer(GameState* g);
void playGame(GameState* g);
void freeGame(GameState* g);

#endif
